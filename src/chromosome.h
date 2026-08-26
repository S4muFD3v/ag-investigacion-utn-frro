#ifndef _CHROMOSOME_H_
#define _CHROMOSOME_H_

#include "types.h"

typedef enum {
    CONSTRAINT_OK = 0,

    CONSTRAINT_R1_COMMISSION_OVERLAP,
    CONSTRAINT_R2_TEACHER_OVERLAP,
    CONSTRAINT_R4_WEEKLY_HOURS,
    CONSTRAINT_R9_INVALID_TEACHER,
    CONSTRAINT_R10_OUTSIDE_SCHEDULE,
    CONSTRAINT_R11_DICTATION_OVERLAP,
    CONSTRAINT_R13_NON_CONSECUTIVE
} constraint_result_t;

typedef struct chromosome_t chromosome_t;
typedef struct gene_t gene_t;

chromosome_t* init_chromosome (size_t geneCount);
void free_chromosome(chromosome_t* chromosome);

constraint_result_t validate_hard_constraints (chromosome_t* chromosome);


#endif 