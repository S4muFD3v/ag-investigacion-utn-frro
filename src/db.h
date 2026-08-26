#ifndef _DB_H_
#define _DB_H_

#include "types.h"

void init_db(void);
void terminate_db(void);

subject_t* create_subject(void);
subject_t* query_subject(id_t subjectId);
subject_t* get_subject_at(size_t position);
size_t get_subject_count(void);

comission_t* create_comission(void);
comission_t* query_comission(id_t comissionId);
comission_t* get_comission_at(size_t position);
size_t get_comission_count(void);

teacher_t* create_teacher(void);
teacher_t* query_teacher(id_t teacherId);
teacher_t* get_teacher_at(size_t position);
size_t get_teacher_count(void);

dictation_t* create_dictation(void);
dictation_t* query_dictation(id_t dictationId);
dictation_t* get_dictation_at(size_t position);
size_t get_dictation_count(void);

block_t* create_block(void);
block_t* query_block(id_t blockId);
block_t* get_block_at(size_t position);
size_t get_block_count(void);

session_t* create_session(void);
session_t* query_session(id_t sessionId);
session_t* get_session_at(size_t position);
size_t get_session_count(void);

#endif
