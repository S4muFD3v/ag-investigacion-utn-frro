#include "db.h"
#include <stdlib.h>
#include <string.h>

#include "utils/memory.h"

struct dbdata_t {
    dynarr_t* subjects;
    dynarr_t* comissions;
    dynarr_t* teachers;
    dynarr_t* dictations;
    dynarr_t* blocks;
} dbdata;



void init_db() {
    dbdata.subjects = init_dynamic_array(sizeof_subject(), 57);//este
    dbdata.comissions = init_dynamic_array(sizeof_comission(), 53); //y este son numeros reales
    dbdata.teachers = init_dynamic_array(sizeof_teacher(), 32);
    dbdata.dictations = init_dynamic_array(sizeof_dictation(), 32);
    dbdata.blocks = init_dynamic_array(sizeof_block(), 32);
}

void terminate_db() {
    for (size_t i = 0; i < get_comission_count(); i++) {
        free(get_comission_subjects(get_comission_at(i)));
    }
    free_dynamic_array(dbdata.subjects);
    free_dynamic_array(dbdata.comissions);
    free_dynamic_array(dbdata.teachers);
    free_dynamic_array(dbdata.dictations);
    free_dynamic_array(dbdata.blocks);
}


subject_t* create_subject() {
    subject_t* subject = push_slot(dbdata.subjects);
    if (subject != NULL) {
        memset(subject, 0, sizeof_subject());
    }
    return subject;
}

subject_t* query_subject(id_t subjectId) {              //probablemente una futura optimizacion venga de poner id's autoincrementales para que esta busqueda sea directa
    for (size_t i = 0; i < get_subject_count(); i++) {
        subject_t* subject = get_subject_at(i);
        if (get_subject_id(subject) == subjectId) {
            return subject;
        }
    }
    return NULL;
}

subject_t* get_subject_at(size_t position) {
    if (position >= get_subject_count()) {
        return NULL;
    }
    return (subject_t*)at(dbdata.subjects, position);
}

subject_t* add_subject(id_t id, const char *name, size_t weeklyBlocks) {
    subject_t *subject = create_subject();

    if (subject == NULL) {
        return NULL;
    }

    set_subject_id(subject, id);
    set_subject_name(subject, name);
    set_subject_weekly_hours(subject, weeklyBlocks);

    return subject;
}

subject_t* get_subjects(void) {
    return get_subject_at(0);
}

size_t get_subject_count() {
    return dynamic_array_size(dbdata.subjects);
}

comission_t* create_comission() {
    comission_t* comission = push_slot(dbdata.comissions);
    if (comission != NULL) {
        memset(comission, 0, sizeof_comission());
    }
    return comission;
}

comission_t* query_comission(id_t comissionId) {
    for (size_t i = 0; i < get_comission_count(); i++) {
        comission_t* comission = get_comission_at(i);
        if (get_comission_id(comission) == comissionId) {
            return comission;
        }
    }
    return NULL;
}

comission_t* get_comission_at(size_t position) {
    if (position >= get_comission_count()) {
        return NULL;
    }
    return (comission_t*)at(dbdata.comissions, position);
}

size_t get_comission_count() {
    return dynamic_array_size(dbdata.comissions);
}

teacher_t* create_teacher() {
    teacher_t* teacher = push_slot(dbdata.teachers);
    if (teacher != NULL) {
        memset(teacher, 0, sizeof_teacher());
    }
    return teacher;
}

teacher_t* query_teacher(id_t teacherId) {
    for (size_t i = 0; i < get_teacher_count(); i++) {
        teacher_t* teacher = get_teacher_at(i);
        if (get_teacher_id(teacher) == teacherId) {
            return teacher;
        }
    }
    return NULL;
}

teacher_t* get_teacher_at(size_t position) {
    if (position >= get_teacher_count()) {
        return NULL;
    }
    return (teacher_t*)at(dbdata.teachers, position);
}

size_t get_teacher_count() {
    return dynamic_array_size(dbdata.teachers);
}

dictation_t* create_dictation() {
    dictation_t* dictation = push_slot(dbdata.dictations);
    if (dictation != NULL) {
        memset(dictation, 0, sizeof_dictation());
    }
    return dictation;
}

dictation_t* query_dictation(id_t dictationId) {
    for (size_t i = 0; i < get_dictation_count(); i++) {
        dictation_t* dictation = get_dictation_at(i);
        if (get_dictation_id(dictation) == dictationId) {
            return dictation;
        }
    }
    return NULL;
}

dictation_t* get_dictation_at(size_t position) {
    if (position >= get_dictation_count()) {
        return NULL;
    }
    return (dictation_t*)at(dbdata.dictations, position);
}

size_t get_dictation_count() {
    return dynamic_array_size(dbdata.dictations);
}

block_t* create_block() {
    block_t* block = push_slot(dbdata.blocks);
    if (block != NULL) {
        memset(block, 0, sizeof_block());
    }
    return block;
}

block_t* query_block(id_t blockId) {
    for (size_t i = 0; i < get_block_count(); i++) {
        block_t* block = get_block_at(i);
        if (get_block_id(block) == blockId) {
            return block;
        }
    }
    return NULL;
}

block_t* get_block_at(size_t position) {
    if (position >= get_block_count()) {
        return NULL;
    }
    return (block_t*)at(dbdata.blocks, position);
}

size_t get_block_count() {
    return dynamic_array_size(dbdata.blocks);
}
