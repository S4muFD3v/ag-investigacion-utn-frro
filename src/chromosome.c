#include "chromosome.h"
#include <stdlib.h>
#include <math.h>
#include "db.h"

struct gene_t {

    id_t sessionId;

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

constraint_result_t validate_hard_constraints (chromosome_t* chromosome){
    
    constraint_result_t result;
    
    result = validate_r1(chromosome);
    if (result != CONSTRAINT_OK){
        return result;
    }

    result = validate_r2(chromosome);
    if (result != CONSTRAINT_OK){
        return result;
    }

    result = validate_r4(chromosome);

    if (result != CONSTRAINT_OK){
        return result;
    }

    result = validate_r9(chromosome);

    if (result != CONSTRAINT_OK){
        return result;
    }

    result = validate_r10(chromosome);

    if (result != CONSTRAINT_OK){
        return result;
    }

    result = validate_r11(chromosome);

    if (result != CONSTRAINT_OK){
        return result;
    }

    result = validate_r13(chromosome);

    if (result != CONSTRAINT_OK){
        return result;
    }
}

constraint_result_t validate_r1(chromosome_t* chromosome){
    for (size_t i=0; i<= chromosome->geneCount; i++){
        session_t* session = query_session(chromosome->genes[i].sessionId);

        dictation_t* dictation = query_dictation(get_session_dictation_id(session));

        comission_t* comission = query_comission(get_dictation_comission_id(dictation));

        id_t comissionId = get_comission_id(comission);

        

    }
}

