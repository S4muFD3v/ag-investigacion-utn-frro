#include "subjectLoader.h"
#include "../db.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>


void load_subjects(const char* subjectFile) {
	FILE* file = fopen(subjectFile, "r");
	fseek(file, 0, SEEK_SET);
	char line[512];
	size_t weeklyHours = 0;
	while (!feof(file)) {
		line[0] = '\0';
		char ch = '\0';
		for (size_t i = 0; i < 512; i++) {
			fread_s(&ch, sizeof(char), sizeof(char), 1, file);
			if (ch == '\n') break;
			line[i] = ch;
		}
		subject_t* subject = create_subject();
		set_subject_name(subject, strtok(line, ";"));
		set_subject_weekly_hours(subject, atoi(strtok(NULL, ";")));
	}
	fclose(file);
}