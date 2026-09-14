#include "subjectLoader.h"
#include "../db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#define SUBJECT_FILE_LINE_MAX 1024

static int read_line(FILE* file, char* line);
static void new_subject_from_line(char* line);

void load_subjects(const char* subjectFile) {
	FILE* file = fopen(subjectFile, "r");
	if (file == NULL) {
		printf("No se pudo abrir el archivo '%s'\n", subjectFile);
		abort();
	}
	char line[SUBJECT_FILE_LINE_MAX];
	while (read_line(file, line)) {
		char* row = line;
		if (strncmp(row, "\xef\xbb\xbf", 3) == 0) row += 3;
		if (row[0] == '\0') continue;
		new_subject_from_line(row);
	}
	if (ferror(file)) {
		fprintf(stderr, "No se pudo leer el archivo '%s'\n", subjectFile);
		fclose(file);
		abort();
	}
	fclose(file);
}


static int read_line(FILE* file, char* line) {
	if (fgets(line, SUBJECT_FILE_LINE_MAX, file) == NULL) return 0;
	line[strcspn(line, "\r\n")] = '\0';
	return 1;
}


static void new_subject_from_line(char* line) {
	char* idField = line;
	char* name = strchr(idField, ';');
	char* hoursField = name == NULL ? NULL : strchr(name + 1, ';');
	if (name == NULL || hoursField == NULL) {
		fprintf(stderr, "Fila de materias incompleta: se espera id;nombre;horas.\n");
		abort();
	}
	*name++ = '\0';
	*hoursField++ = '\0';
	hoursField[strcspn(hoursField, ";")] = '\0';
	if (idField[0] == '\0' || name[0] == '\0' || hoursField[0] == '\0') {
		fprintf(stderr, "Fila de materias con un campo obligatorio vacio.\n");
		abort();
	}

	char* idEnd;
	errno = 0;
	id_t id = strtoll(idField, &idEnd, 10);
	int validId = errno == 0 && idEnd != idField;
	while (*idEnd == ' ' || *idEnd == '\t') idEnd++;
	validId = validId && *idEnd == '\0';
	char* hoursEnd;
	errno = 0;
	unsigned long long weeklyHours = strtoull(hoursField, &hoursEnd, 10);
	int validHours = errno == 0 && hoursEnd != hoursField;
	while (*hoursEnd == ' ' || *hoursEnd == '\t') hoursEnd++;
	if (!validId || !validHours || *hoursEnd != '\0' ||
		hoursField[strspn(hoursField, " \t")] == '-' || weeklyHours > SIZE_MAX) {
		fprintf(stderr, "ID u horas invalidos en la materia '%s'.\n", name);
		abort();
	}

	subject_t* subject = create_subject();
	if (subject == NULL) {
		fprintf(stderr, "No se pudo reservar memoria para la materia '%s'.\n", name);
		abort();
	}
	set_subject_id(subject, id);
	set_subject_name(subject, name);
	set_subject_weekly_hours(subject, (size_t)weeklyHours);
}
