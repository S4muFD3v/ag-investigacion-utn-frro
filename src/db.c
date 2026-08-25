#include "db.h"
#include <stdlib.h>

#include "utils/memory.h"

struct dbdata_t {
    dynarr_t* subjects;
    dynarr_t* comissions;
} dbdata;



void init_db() {
    dbdata.subjects = init_dynamic_array(sizeof_subject(), 32);
    dbdata.comissions = init_dynamic_array(sizeof_comission(), 16);
}

void terminate_db() {

}


subject_t* create_subject() {

}

subject_t* query_subject(id_t subjectId) {

}