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
    char name[COMISSION_NAME_MAX_LENGTH];
    size_t year;
    schedule_t first_period;

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

id_t get_subject_id(subject_t* subject) {
    return subject->id;
}

void set_subject_id(subject_t* subject, id_t id) {
    subject->id = id;
}

char* get_subject_name(subject_t* subject) {
    return subject->name;
}

void set_subject_name(subject_t* subject, const char* name) {
    if (name == NULL) abort();

    strncpy(subject->name, name, SUBJECT_NAME_MAX_LENGTH - 1);
    subject->name[SUBJECT_NAME_MAX_LENGTH - 1] = '\0';
}

size_t get_subject_weekly_hours(subject_t* subject) {
    return subject->weeklyHours;
}

void set_subject_weekly_hours(subject_t* subject, size_t weeklyHours) {
    subject->weeklyHours = weeklyHours;
}

size_t sizeof_subject() {
    return sizeof(subject_t);
}

id_t get_com_subject_id(com_subjects_t* comSubject) {
    return comSubject->subjectId;
}

void set_com_subject_id(com_subjects_t* comSubject, id_t subjectId) {
    comSubject->subjectId = subjectId;
}

fmp_t get_com_subject_q(com_subjects_t* comSubject) {
    return comSubject->q;
}

void set_com_subject_q(com_subjects_t* comSubject, fmp_t q) {
    comSubject->q = q;
}

size_t sizeof_com_subject() {
    return sizeof(com_subjects_t);
}

id_t get_comission_id(comission_t* comission) {
    return comission->id;
}

void set_comission_id(comission_t* comission, id_t id) {
    comission->id = id;
}

char* get_comission_name(comission_t* comission) {
    return comission->name;
}

void set_comission_name(comission_t* comission, const char* name) {
    if (name == NULL) abort();

    strncpy(comission->name, name, COMISSION_NAME_MAX_LENGTH - 1);
    comission->name[COMISSION_NAME_MAX_LENGTH - 1] = '\0';
}

size_t get_comission_year(comission_t* comission) {
    return comission->year;
}

void set_comission_year(comission_t* comission, size_t year) {
    comission->year = year;
}

schedule_t get_comission_schedule(comission_t* comission) {
    return comission->first_period;
}

void set_comission_schedule(comission_t* comission, schedule_t first_period) {
    comission->first_period = first_period;
}

com_subjects_t* get_comission_subjects(comission_t* comission) {
    return comission->subjects;
}

void set_comission_subjects(comission_t* comission, com_subjects_t* subjects) {
    comission->subjects = subjects;
}

com_subjects_t* get_comission_subject_at(comission_t* comission, size_t position) {
    if (position >= comission->length) {
        return NULL;
    }
    return comission->subjects + position;
}

size_t get_comission_length(comission_t* comission) {
    return comission->length;
}

void set_comission_length(comission_t* comission, size_t length) {
    comission->length = length;
}

size_t sizeof_comission() {
    return sizeof(comission_t);
}

id_t get_teacher_id(teacher_t* teacher) {
    return teacher->id;
}

void set_teacher_id(teacher_t* teacher, id_t id) {
    teacher->id = id;
}

char* get_teacher_name(teacher_t* teacher) {
    return teacher->name;
}

void set_teacher_name(teacher_t* teacher, char* name) {
    if (name == NULL) abort();

    strncpy(teacher->name, name, TEACHER_NAME_MAX_LENGTH - 1);
    teacher->name[TEACHER_NAME_MAX_LENGTH - 1] = '\0';
}

size_t sizeof_teacher() {
    return sizeof(teacher_t);
}

id_t get_dictation_id(dictation_t* dictation) {
    return dictation->id;
}

void set_dictation_id(dictation_t* dictation, id_t id) {
    dictation->id = id;
}

id_t get_dictation_subject_id(dictation_t* dictation) {
    return dictation->subjectId;
}

void set_dictation_subject_id(dictation_t* dictation, id_t subjectId) {
    dictation->subjectId = subjectId;
}

id_t get_dictation_comission_id(dictation_t* dictation) {
    return dictation->comissionId;
}

void set_dictation_comission_id(dictation_t* dictation, id_t comissionId) {
    dictation->comissionId = comissionId;
}

id_t get_dictation_teacher_id(dictation_t* dictation) {
    return dictation->teacherId;
}

void set_dictation_teacher_id(dictation_t* dictation, id_t teacherId) {
    dictation->teacherId = teacherId;
}

size_t sizeof_dictation() {
    return sizeof(dictation_t);
}

id_t get_block_id(block_t* block) {
    return block->id;
}

void set_block_id(block_t* block, id_t id) {
    block->id = id;
}

size_t get_block_start_minutes(block_t* block) {
    return block->startMinutes;
}

void set_block_start_minutes(block_t* block, size_t startMinutes) {
    block->startMinutes = startMinutes;
}

size_t get_block_end_minutes(block_t* block) {
    return block->endMinutes;
}

void set_block_end_minutes(block_t* block, size_t endMinutes) {
    block->endMinutes = endMinutes;
}

schedule_t get_block_schedules(block_t* block) {
    return block->schedules;
}

void set_block_schedules(block_t* block, schedule_t schedules) {
    block->schedules = schedules;
}

size_t sizeof_block() {
    return sizeof(block_t);
}
