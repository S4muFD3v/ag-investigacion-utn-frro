#include "utils/r7_matrix.h"

#include <stdio.h>

static void write_csv_quoted(FILE* file, const char* value) {
    fputc('"', file);
    while (*value != '\0') {
        if (*value == '"') {
            fputc('"', file);
        }
        fputc(*value, file);
        value++;
    }
    fputc('"', file);
}

static void write_prerequisites(FILE* file, id_t subjectId, int approved) {
    int wroteValue = 0;
    fputc('"', file);

    for (size_t i = 0; i < r7_subject_count(); i++) {
        const r7_subject_info_t* prerequisite = r7_subject_at(i);
        int matches = approved
            ? r7_is_direct_approved_prerequisite(subjectId, prerequisite->id)
            : r7_is_direct_regular_prerequisite(subjectId, prerequisite->id);

        if (!matches) {
            continue;
        }
        if (wroteValue) {
            fputc('|', file);
        }
        fputs(prerequisite->planCode, file);
        wroteValue = 1;
    }

    fputc('"', file);
}

static int export_catalog(const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }

    fputs("subject_id,plan_code,name,year,type,regular_prerequisites,approved_prerequisites\n",
          file);

    for (size_t i = 0; i < r7_subject_count(); i++) {
        const r7_subject_info_t* subject = r7_subject_at(i);
        fprintf(file, "%lld,", (long long)subject->id);
        write_csv_quoted(file, subject->planCode);
        fputc(',', file);
        write_csv_quoted(file, subject->name);
        fprintf(file, ",%zu,", subject->year);
        write_csv_quoted(file, r7_subject_kind_name(subject->kind));
        fputc(',', file);
        write_prerequisites(file, subject->id, 0);
        fputc(',', file);
        write_prerequisites(file, subject->id, 1);
        fputc('\n', file);
    }

    return fclose(file) == 0;
}

static int export_matrix(const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }

    fputs("subject_id,plan_code,subject_name", file);
    for (size_t column = 0; column < r7_subject_count(); column++) {
        const r7_subject_info_t* subject = r7_subject_at(column);
        fputc(',', file);
        write_csv_quoted(file, subject->planCode);
    }
    fputc('\n', file);

    for (size_t row = 0; row < r7_subject_count(); row++) {
        const r7_subject_info_t* subject = r7_subject_at(row);
        fprintf(file, "%lld,", (long long)subject->id);
        write_csv_quoted(file, subject->planCode);
        fputc(',', file);
        write_csv_quoted(file, subject->name);

        for (size_t column = 0; column < r7_subject_count(); column++) {
            fprintf(file, ",%d", r7_conflict_score_by_index(row, column));
        }
        fputc('\n', file);
    }

    return fclose(file) == 0;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <catalog.csv> <matrix.csv>\n", argv[0]);
        return 1;
    }

    if (!export_catalog(argv[1])) {
        fprintf(stderr, "Could not export catalog to %s\n", argv[1]);
        return 1;
    }
    if (!export_matrix(argv[2])) {
        fprintf(stderr, "Could not export matrix to %s\n", argv[2]);
        return 1;
    }

    return 0;
}
