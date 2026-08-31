#include "algGen.h"
#include "chromosome.h"
#include <stdlib.h>
#include "db.h"

enum {
    DAY_COUNT = 5,
    BLOCK_COUNT = 8
};

static gene_t* gene_at(gene_t* firstGene, size_t geneSize, size_t position) {
    return (gene_t*)((char*)firstGene + geneSize * position);
}

static int randomize_dictation(gene_t* firstGene, size_t geneSize, size_t firstPosition, size_t sessionCount, size_t weeklyBlocks) {
    if (sessionCount == 0 || weeklyBlocks > sessionCount * BLOCK_COUNT) {
        return 0;
    }

    for (size_t i = 0; i < sessionCount; i++) {
        gene_t* gene = gene_at(firstGene, geneSize, firstPosition + i);
        set_gene_day(gene, 0);
        set_gene_start_block_id(gene, 0);
        set_gene_length(gene, 0);
    }

    for (size_t block = 0; block < weeklyBlocks; block++) {
        size_t sessionPosition = (size_t)rand() % sessionCount;
        size_t checkedSessions = 0;
        gene_t* gene = gene_at(firstGene, geneSize,
                               firstPosition + sessionPosition);

        while (get_gene_length(gene) == BLOCK_COUNT &&
               checkedSessions < sessionCount) {
            sessionPosition = (sessionPosition + 1) % sessionCount;
            checkedSessions++;
            gene = gene_at(firstGene, geneSize,
                           firstPosition + sessionPosition);
        }

        if (checkedSessions == sessionCount) {
            return 0;
        }

        set_gene_length(gene, get_gene_length(gene) + 1);
    }

    for (size_t i = 0; i < sessionCount; i++) {
        gene_t* gene = gene_at(firstGene, geneSize, firstPosition + i);
        size_t length = get_gene_length(gene);

        if (length == 0) {
            continue;
        }

        set_gene_day(gene, (size_t)rand() % DAY_COUNT);
        set_gene_start_block_id(
            gene,
            (id_t)((size_t)rand() % (BLOCK_COUNT - length + 1))
        );
    }

    return 1;
}

population_t* init_population(const chromosome_t* initial) {
    population_t* population =
        calloc(POPULATION_SIZE, sizeof(*population));

    if (population == NULL) {
        return NULL;
    }

    population[0] = clone_chromosome(initial);

    if (population[0] == NULL) {
        free(population);
        return NULL;
    }

    sort_chromosome_by_dictation(population[0]);

    return population;
}

void free_population(population_t *population) {
    if (population == NULL) {
        return;
    }

    for (size_t i = 0; i < POPULATION_SIZE; i++) {
        free_chromosome(population[i]);
    }

    free(population);
}

void create_first_population(population_t* population) {
    if (population == NULL || population[0] == NULL) {
        return;
    }

    size_t geneCount = get_chromosome_gene_count(population[0]);
    size_t geneSize = get_sizeof_genes();

    for (size_t i = 1; i < POPULATION_SIZE; i++) {
        population[i] = clone_chromosome(population[0]);
        if (population[i] == NULL) {
            return;
        }

        gene_t* firstGene = get_genes(population[i]);
        size_t groupStart = 0;

        while (groupStart < geneCount) {
            gene_t* firstGroupGene = gene_at(firstGene, geneSize, groupStart);
            id_t dictationId = get_gene_dictation_id(firstGroupGene);
            size_t groupEnd = groupStart + 1;

            while (groupEnd < geneCount) {
                gene_t* nextGene = gene_at(firstGene, geneSize, groupEnd);
                if (get_gene_dictation_id(nextGene) != dictationId) {
                    break;
                }
                groupEnd++;
            }

            dictation_t* dictation = query_dictation(dictationId);
            subject_t* subject = dictation == NULL ? NULL : query_subject(get_dictation_subject_id(dictation));

            if (subject == NULL || !randomize_dictation(firstGene, geneSize, groupStart, (groupEnd - groupStart), get_subject_weekly_hours(subject))) {
                free_chromosome(population[i]);
                population[i] = NULL;
                return;
            }

            groupStart = groupEnd;
        }
    }
}



