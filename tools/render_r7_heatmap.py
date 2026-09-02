#!/usr/bin/env python3

import csv
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


COLOR_STOPS = (
    (0, (35, 43, 53)),
    (25, (35, 150, 140)),
    (50, (238, 202, 73)),
    (75, (229, 112, 55)),
    (100, (179, 48, 67)),
)


def load_font(size, bold=False):
    filename = "DejaVuSans-Bold.ttf" if bold else "DejaVuSans.ttf"
    candidates = (
        Path("/usr/share/fonts/truetype/dejavu") / filename,
        Path("C:/Windows/Fonts") / ("arialbd.ttf" if bold else "arial.ttf"),
    )
    for candidate in candidates:
        if candidate.exists():
            return ImageFont.truetype(str(candidate), size)
    return ImageFont.load_default()


def interpolate(first, second, ratio):
    return tuple(round(a + (b - a) * ratio) for a, b in zip(first, second))


def score_color(score):
    for index in range(len(COLOR_STOPS) - 1):
        low_value, low_color = COLOR_STOPS[index]
        high_value, high_color = COLOR_STOPS[index + 1]
        if score <= high_value:
            ratio = (score - low_value) / (high_value - low_value)
            return interpolate(low_color, high_color, ratio)
    return COLOR_STOPS[-1][1]


def read_catalog(path):
    with path.open(newline="", encoding="utf-8") as file:
        return list(csv.DictReader(file))


def read_matrix(path):
    with path.open(newline="", encoding="utf-8") as file:
        reader = csv.reader(file)
        header = next(reader)
        rows = list(reader)
    codes = header[3:]
    values = [[int(value) for value in row[3:]] for row in rows]
    return codes, values


def render(catalog_path, matrix_path, output_path):
    catalog = read_catalog(catalog_path)
    codes, matrix = read_matrix(matrix_path)
    count = len(codes)

    title_font = load_font(30, bold=True)
    subtitle_font = load_font(16)
    axis_font = load_font(9)
    body_font = load_font(14)
    body_bold_font = load_font(14, bold=True)

    kind_labels = {"required": "T", "seminar": "S", "elective": "E"}
    catalog_labels = [
        (
            f'{int(subject["subject_id"]):02d} [{subject["plan_code"]}] '
            f'{subject["name"]} ({subject["year"]}A, '
            f'{kind_labels[subject["type"]]})'
        )
        for subject in catalog
    ]

    cell = 22
    matrix_left = 72
    matrix_top = 104
    matrix_size = count * cell
    catalog_left = matrix_left + matrix_size + 64
    rows_per_column = (len(catalog) + 1) // 2
    first_column_width = max(
        body_font.getlength(label)
        for label in catalog_labels[:rows_per_column]
    ) + 38
    second_column_width = max(
        body_font.getlength(label)
        for label in catalog_labels[rows_per_column:]
    )
    width = int(catalog_left + first_column_width + second_column_width + 48)
    height = max(matrix_top + matrix_size + 100, 1120)

    image = Image.new("RGB", (width, height), (247, 248, 250))
    draw = ImageDraw.Draw(image)

    draw.text((matrix_left, 24), "R7 - Matriz de conflicto entre materias",
              fill=(24, 31, 40), font=title_font)
    draw.text((matrix_left, 64),
              "0: sin penalizacion por correlatividad  |  100: evitar fuertemente la superposicion",
              fill=(72, 81, 93), font=subtitle_font)

    for row in range(count):
        for column in range(count):
            x = matrix_left + column * cell
            y = matrix_top + row * cell
            draw.rectangle((x, y, x + cell - 1, y + cell - 1),
                           fill=score_color(matrix[row][column]))

    for index, subject in enumerate(catalog):
        label = subject["subject_id"]
        x = matrix_left + index * cell + cell // 2
        y = matrix_top + index * cell + cell // 2
        draw.text((x, matrix_top - 8), label, fill=(49, 57, 68),
                  font=axis_font, anchor="ms")
        draw.text((matrix_left - 7, y), label, fill=(49, 57, 68),
                  font=axis_font, anchor="rm")

    legend_y = matrix_top + matrix_size + 42
    legend_width = 360
    for offset in range(legend_width):
        score = round(offset * 100 / (legend_width - 1))
        draw.line((matrix_left + offset, legend_y,
                   matrix_left + offset, legend_y + 18),
                  fill=score_color(score))
    draw.text((matrix_left, legend_y + 25), "0", fill=(49, 57, 68),
              font=body_font)
    draw.text((matrix_left + legend_width, legend_y + 25), "100",
              fill=(49, 57, 68), font=body_font, anchor="ra")

    draw.text((catalog_left, matrix_top), "Catalogo canonico",
              fill=(24, 31, 40), font=body_bold_font)
    line_height = 31

    for index, label in enumerate(catalog_labels):
        column = index // rows_per_column
        row = index % rows_per_column
        x = catalog_left + column * first_column_width
        y = matrix_top + 34 + row * line_height
        draw.text((x, y), label, fill=(49, 57, 68), font=body_font)

    image.save(output_path)


def main():
    if len(sys.argv) != 4:
        print(
            "Usage: render_r7_heatmap.py <catalog.csv> <matrix.csv> <output.png>",
            file=sys.stderr,
        )
        return 1

    render(Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
