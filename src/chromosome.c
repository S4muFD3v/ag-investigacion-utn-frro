#include "chromosome.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "db.h"

struct gene_t {

    id_t dictationId;

    size_t day;
    id_t startBlockId;
    size_t length;
};

struct chromosome_t {
    gene_t* genes;
    size_t geneCount;
    double fitness;
};

static int compare_genes_by_dictation(const void* a, const void* b) {
    const gene_t* first = a;
    const gene_t* second = b;

    if (first->dictationId < second->dictationId) {
        return -1;
    }
    if (first->dictationId > second->dictationId) {
        return 1;
    }
    return 0;
}

chromosome_t* init_chromosome(size_t geneCount) {
    chromosome_t* chromosome = malloc(sizeof(chromosome_t));

    if (chromosome == NULL) {
        return NULL;
    }

    chromosome->genes = calloc(geneCount, sizeof(gene_t));

    if (chromosome->genes == NULL) {
        free(chromosome);
        return NULL;
    }

    chromosome->geneCount = geneCount;
    chromosome->fitness = INFINITY;

    return chromosome;
}

chromosome_t* clone_chromosome(const chromosome_t* source) {
    if (source == NULL) {
        return NULL;
    }

    chromosome_t* clone = init_chromosome(source->geneCount);
    if (clone == NULL) {
        return NULL;
    }

    memcpy(clone->genes, source->genes,
           source->geneCount * sizeof(gene_t));
    clone->fitness = source->fitness;

    return clone;
}

void sort_chromosome_by_dictation(chromosome_t* chromosome) {
    if (chromosome == NULL || chromosome->geneCount < 2) {
        return;
    }

    qsort(chromosome->genes, chromosome->geneCount, sizeof(gene_t),
          compare_genes_by_dictation);
}

void free_chromosome(chromosome_t* chromosome) {
    if (chromosome == NULL) {
        return;
    }

    free(chromosome->genes);
    free(chromosome);
}

size_t get_chromosome_gene_count(const chromosome_t* chromosome) {
    return chromosome->geneCount;
}

gene_t* get_gene_at(chromosome_t* chromosome, size_t position) {
    if (position >= chromosome->geneCount) {
        return NULL;
    }
    return chromosome->genes + position;
}

double get_chromosome_fitness(const chromosome_t* chromosome) {
    return chromosome->fitness;
}

void set_chromosome_fitness(chromosome_t* chromosome, double fitness) {
    chromosome->fitness = fitness;
}

void init_gene(gene_t* gene, id_t dictationId, size_t day, id_t startBlockId, size_t length) {
    gene->dictationId = dictationId;
    gene->day = day;
    gene->startBlockId = startBlockId;
    gene->length = length;
}

id_t get_gene_dictation_id(const gene_t* gene) {
    return gene->dictationId;
}

void set_gene_dictation_id(gene_t* gene, id_t id) {
    gene->dictationId = id;
}

size_t get_gene_day(const gene_t* gene) {
    return gene->day;
}

void set_gene_day(gene_t* gene, size_t day) {
    gene->day = day;
}

id_t get_gene_start_block_id(const gene_t* gene) {
    return gene->startBlockId;
}

void set_gene_start_block_id(gene_t* gene, id_t startBlockId) {
    gene->startBlockId = startBlockId;
}

size_t get_gene_length(const gene_t* gene) {
    return gene->length;
}

void set_gene_length(gene_t* gene, size_t length) {
    gene->length = length;
}

id_t get_comission_id_from_gene(const gene_t* gene) {
    if (gene == NULL) {
        return -1;
    }

    id_t dictationId = gene->dictationId;
    dictation_t* dictation = query_dictation(dictationId);
    id_t comissionId = dictation == NULL ? -1 : get_dictation_comission_id(dictation);
    return comissionId;
}

id_t get_teacher_id_from_gene(const gene_t* gene) {
    if (gene == NULL) {
        return -1;
    }

    id_t dictationId = gene->dictationId;
    dictation_t* dictation = query_dictation(dictationId);
    id_t teacherId = dictation == NULL ? -1 : get_dictation_teacher_id(dictation);
    return teacherId;
}

id_t get_subject_id_from_gene(const gene_t* gene) {
    if (gene == NULL) {
        return -1;
    }

    id_t dictationId = gene->dictationId;
    dictation_t* dictation = query_dictation(dictationId);
    id_t subjectId = dictation == NULL ? -1 : get_dictation_subject_id(dictation);
    return subjectId;
}

fmp_t get_fmp_from_gene(const gene_t* gene) {
    if (gene == NULL) {
        return FMP_MAX_ENUM;
    }

    dictation_t* dictation = query_dictation(gene->dictationId);
    if (dictation == NULL) {
        return FMP_MAX_ENUM;
    }

    id_t subjectId = get_dictation_subject_id(dictation);
    comission_t* comission = query_comission(
        get_dictation_comission_id(dictation));
    if (comission == NULL) {
        return FMP_MAX_ENUM;
    }

    size_t subjectCount = get_comission_length(comission);
    for (size_t i = 0; i < subjectCount; i++) {
        com_subjects_t* comSubject = get_comission_subject_at(comission, i);
        if (get_com_subject_id(comSubject) == subjectId) {
            return get_com_subject_q(comSubject);
        }
    }

    return FMP_MAX_ENUM;
}
