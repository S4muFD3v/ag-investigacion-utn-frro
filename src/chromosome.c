#include "chromosome.h"
#include <stdlib.h>

struct gene_t {
    size_t day;
    id_t startBlockId;
    size_t length;
};

struct chromosome_t {
    gene_t* genes;
    size_t geneCount;
    double fitness;
};

chromosome_t* init_chromosome (){
    chromosome_t* chromosome = malloc(sizeof(chromosome_t));
    chromosome->genes = malloc(sizeof(gene_t) * sizeof_session())
} 