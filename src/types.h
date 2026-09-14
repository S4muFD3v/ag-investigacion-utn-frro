#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>

#define SUBJECT_NAME_MAX_LENGTH 256
#define TEACHER_NAME_MAX_LENGTH 256

typedef long long int id_t;
typedef struct subject_t subject_t;
typedef struct comission_t comission_t;
typedef struct block_t block_t;
typedef struct teacher_t teacher_t;
typedef struct dictation_t dictation_t;
typedef struct com_subjects_t com_subjects_t;

typedef enum {
    SCHEDULE_MORNING = 0x0000000000000001,
    SCHEDULE_AFTERNOON = 0x0000000000000002,
    SCHEDULE_EVENING = 0x0000000000000004,
    SCHEDULE_MAX_ENUM = 0xfffffffffffffff8
} schedule_t;

typedef enum { 
    FMP_FIRST = 0x0000000000000001,
    FMP_SECOND = 0x0000000000000002,
    FMP_BOTH = 0x0000000000000004,
    FMP_MAX_ENUM = 0xfffffffffffffff8
} fmp_t;

/* SUBJECT */

id_t get_subject_id(subject_t* subject);
void set_subject_id(subject_t* subject, id_t id);

char* get_subject_name(subject_t* subject);
void set_subject_name(subject_t* subject, const char* name);

size_t get_subject_weekly_hours(subject_t* subject);
void set_subject_weekly_hours(subject_t* subject, size_t weeklyHours);

size_t sizeof_subject();


/* COM_SUBJECTS */

id_t get_com_subject_id(com_subjects_t* comSubject);
void set_com_subject_id(com_subjects_t* comSubject, id_t subjectId);

fmp_t get_com_subject_q(com_subjects_t* comSubject);
void set_com_subject_q(com_subjects_t* comSubject, fmp_t q);

size_t sizeof_com_subject();


/* COMISSION */

id_t get_comission_id(comission_t* comission);
void set_comission_id(comission_t* comission, id_t id);

size_t get_comission_year(comission_t* comission);
void set_comission_year(comission_t* comission, size_t year);

schedule_t get_comission_schedule(comission_t* comission);
void set_comission_schedule(comission_t* comission, schedule_t first_period);

com_subjects_t* get_comission_subjects(comission_t* comission);
/* La comision conserva el arreglo reservado en heap; terminate_db lo libera. */
void set_comission_subjects(comission_t* comission, com_subjects_t* subjects);
com_subjects_t* get_comission_subject_at(comission_t* comission, size_t position);

size_t get_comission_length(comission_t* comission);
void set_comission_length(comission_t* comission, size_t length);

size_t sizeof_comission();


/* TEACHER */

id_t get_teacher_id(teacher_t* teacher);
void set_teacher_id(teacher_t* teacher, id_t id);

char* get_teacher_name(teacher_t* teacher);
void set_teacher_name(teacher_t* teacher, char* name);

size_t sizeof_teacher();


/* DICTATION */

id_t get_dictation_id(dictation_t* dictation);
void set_dictation_id(dictation_t* dictation, id_t id);

id_t get_dictation_subject_id(dictation_t* dictation);
void set_dictation_subject_id(dictation_t* dictation, id_t subjectId);

id_t get_dictation_comission_id(dictation_t* dictation);
void set_dictation_comission_id(dictation_t* dictation, id_t comissionId);

id_t get_dictation_teacher_id(dictation_t* dictation);
void set_dictation_teacher_id(dictation_t* dictation, id_t teacherId);

size_t sizeof_dictation();


/* BLOCK */

id_t get_block_id(block_t* block);
void set_block_id(block_t* block, id_t id);

size_t get_block_start_minutes(block_t* block);
void set_block_start_minutes(block_t* block, size_t startMinutes);

size_t get_block_end_minutes(block_t* block);
void set_block_end_minutes(block_t* block, size_t endMinutes);

schedule_t get_block_schedules(block_t* block);
void set_block_schedules(block_t* block, schedule_t schedules);

size_t sizeof_block();

#endif
