#include "types.h"
#include <stdlib.h>
#include <string.h>

struct subject_t {
    id_t id;
    char name[SUBJECT_NAME_MAX_LENGTH];
    size_t weeklyHours;
};

struct com_subjects_t {
    id_t subjectId;
    fmp_t q;
};

struct comission_t {
    id_t id;
    size_t year;
    schedule_t schedule;

    com_subjects_t *subjects;
    size_t length;
};

struct teacher_t {
    id_t id;
    char name[TEACHER_NAME_MAX_LENGTH];
};

struct dictation_t {
    id_t id;
    id_t subjectId;
    id_t comissionId;
    id_t teacherId;
};

struct block_t {
    id_t id;

    size_t startMinutes;
    size_t endMinutes;

    schedule_t schedules; //esto sirve para los bloques compartidos, por ejemplo, el bloque que comparte entre mañana y tarde tendria 
    //schedules = SCHEDULE_MORNING | SCHEDULE_AFTERNOON y si es uno normal seria schedules = SCHEDULE_MORNING
};

struct session_t {
    id_t id;

    id_t dictationId;

    size_t day;
    id_t startBlockId;
    size_t length;
};

char* get_subject_name(subject_t* subject) {
    return subject->name;
}

size_t get_subject_weekly_hours(subject_t* subject) {
    return subject->weeklyHours;
}

void set_subject_name(subject_t* subject, char* name) {
    errno_t status = strcpy_s(subject->name, SUBJECT_NAME_MAX_LENGTH, name);
    if(status != 0) abort();
}

void set_subject_weekly_hours(subject_t* subject, size_t weeklyHours) {
    subject->weeklyHours = weeklyHours;
}

size_t sizeof_subject() {
    return sizeof(subject_t);
}

