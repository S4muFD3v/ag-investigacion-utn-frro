#ifndef R7_MATRIX_H
#define R7_MATRIX_H

#include "../types.h"

#define R7_SUBJECT_COUNT 57
#define R7_CONFLICT_MAX 100
#define R7_CONFLICT_UNKNOWN (-1)

typedef enum {
    R7_SUBJECT_REQUIRED,
    R7_SUBJECT_SEMINAR,
    R7_SUBJECT_ELECTIVE
} r7_subject_kind_t;

typedef struct {
    id_t id;
    const char* planCode;
    const char* name;
    size_t year;
    r7_subject_kind_t kind;
} r7_subject_info_t;

size_t r7_subject_count(void);
const r7_subject_info_t* r7_subject_at(size_t index);
const r7_subject_info_t* r7_subject_by_id(id_t subjectId);
const r7_subject_info_t* r7_subject_by_plan_code(const char* planCode);
const char* r7_subject_kind_name(r7_subject_kind_t kind);

int r7_is_direct_regular_prerequisite(id_t subjectId, id_t prerequisiteId);
int r7_is_direct_approved_prerequisite(id_t subjectId, id_t prerequisiteId);
int r7_requires_subject(id_t subjectId, id_t prerequisiteId);

int r7_conflict_score(id_t firstSubjectId, id_t secondSubjectId);
int r7_conflict_score_by_index(size_t firstIndex, size_t secondIndex);
double r7_conflict_weight(id_t firstSubjectId, id_t secondSubjectId);

#endif
