#include "chromosome.h"
#include <stdlib.h>
#include <math.h>

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

void init_gene(gene_t* gene, id_t dictationId, size_t day,
               id_t startBlockId, size_t length) {
    gene->dictationId = dictationId;
    gene->day = day;
    gene->startBlockId = startBlockId;
    gene->length = length;
}

id_t get_gene_dictation_id(const gene_t* gene) {
    return gene->dictationId;
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
