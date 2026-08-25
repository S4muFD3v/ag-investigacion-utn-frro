#ifndef _DB_H_
#define _DB_H_

#include "types.h"

void init_db();
void terminate_db();

subject_t* create_subject();
subject_t* query_subject(id_t subjectId);



#endif