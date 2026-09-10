#include "subjectLoader.h"
#include "../db.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>


void load_subjects(const char* subjectFile) {
	FILE* file = fopen(subjectFile, "r");
	if (file == NULL) {
		printf("No se pudo abrir el archivo\n");
		abort();
	}
	fseek(file, 0, SEEK_SET);
	char line[512];
	size_t weeklyHours = 0;
	while (!feof(file)) {
		line[0] = '\0';
		char ch = '\0';
		size_t i;
		for (i = 0; i < 511; i++) {
			fread_s(&ch, sizeof(char), sizeof(char), 1, file);
			if (ch == '\n') break;
			line[i] = ch;
		}
		line[i] = '\0';
		if (feof(file)) break;
		subject_t* subject = create_subject();
		set_subject_name(subject, strtok(line, ";"));
		set_subject_weekly_hours(subject, atoi(strtok(NULL, ";")));
	}
	fclose(file);
}