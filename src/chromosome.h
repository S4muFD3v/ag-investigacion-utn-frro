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
chromosome_t* clone_chromosome(const chromosome_t* source);
void sort_chromosome_by_dictation(chromosome_t* chromosome);
void free_chromosome(chromosome_t* chromosome);

size_t get_chromosome_gene_count(const chromosome_t* chromosome);
gene_t* get_gene_at(chromosome_t* chromosome, size_t position);

double get_chromosome_fitness(const chromosome_t* chromosome);
void set_chromosome_fitness(chromosome_t* chromosome, double fitness);

void init_gene(gene_t* gene, id_t dictationId, size_t day,
               id_t startBlockId, size_t length);

id_t get_gene_dictation_id(const gene_t* gene);

size_t get_gene_day(const gene_t* gene);
void set_gene_day(gene_t* gene, size_t day);

id_t get_gene_start_block_id(const gene_t* gene);
void set_gene_start_block_id(gene_t* gene, id_t startBlockId);

size_t get_gene_length(const gene_t* gene);
void set_gene_length(gene_t* gene, size_t length);
id_t get_comission_id_from_gene(const gene_t* gene);
id_t get_teacher_id_from_gene(const gene_t* gene);

#endif
