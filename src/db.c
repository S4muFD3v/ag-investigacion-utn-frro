#include "db.h"
#include <stdlib.h>

#include "utils/memory.h"

struct dbdata_t {
    dynarr_t* subjects;
    dynarr_t* comissions;
    dynarr_t* teachers;
    dynarr_t* dictations;
    dynarr_t* blocks;
    dynarr_t* sessions;
} dbdata;



void init_db() {
    dbdata.subjects = init_dynamic_array(sizeof_subject(), 57);//este
    dbdata.comissions = init_dynamic_array(sizeof_comission(), 53); //y este son numeros reales
    dbdata.teachers = init_dynamic_array(sizeof_teacher(), 32);
    dbdata.dictations = init_dynamic_array(sizeof_dictation(), 32);
    dbdata.blocks = init_dynamic_array(sizeof_block(), 32);
    dbdata.sessions = init_dynamic_array(sizeof_session(), 32);
}

void terminate_db() {
    free_dynamic_array(dbdata.subjects);
    free_dynamic_array(dbdata.comissions);
    free_dynamic_array(dbdata.teachers);
    free_dynamic_array(dbdata.dictations);
    free_dynamic_array(dbdata.blocks);
    free_dynamic_array(dbdata.sessions);
}


subject_t* create_subject() {

}

subject_t* query_subject(id_t subjectId) {

}