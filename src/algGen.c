#include "algGen.h"
#include "chromosome.h"
#include <stdlib.h>
#include "db.h"
#include "utils/memory.h"
#include "utils/r7_matrix.h"
#include <math.h>

static int validation_overlaps(chromosome_t* chromosome, size_t firstPosition, size_t lastPosition) {
    
    size_t size = lastPosition - firstPosition;
    size_t* day = (size_t*)calloc(size, sizeof(size_t));
    id_t* startBlockId = (id_t*)calloc(size, sizeof(id_t));
    size_t* length = (size_t*)calloc(size, sizeof(size_t));

    if (day == NULL || startBlockId == NULL || length == NULL) {
        free(day);
        free(startBlockId);
        free(length);
        return 0;
    }

    id_t comissionId = get_comission_id_from_gene(get_gene_at(chromosome, firstPosition));
    id_t teacherId = get_teacher_id_from_gene(get_gene_at(chromosome, firstPosition));

    for (size_t i = firstPosition; i < lastPosition; i++) {
        gene_t* gene = get_gene_at(chromosome, i);
        day[i - firstPosition] = get_gene_day(gene);
        startBlockId[i - firstPosition] = get_gene_start_block_id(gene);
        length[i - firstPosition] = get_gene_length(gene);   
    }

    for (size_t j = 0; j < firstPosition; j++) {
        gene_t* geneCheck = get_gene_at(chromosome, j);
        size_t dayCheck = get_gene_day(geneCheck);
        id_t startBlockIdCheck = get_gene_start_block_id(geneCheck);
        size_t lengthCheck = get_gene_length(geneCheck);
        id_t comissionIdCheck = get_comission_id_from_gene(geneCheck);
        id_t teacherIdCheck = get_teacher_id_from_gene(geneCheck);
        
        if (comissionIdCheck == comissionId || teacherIdCheck == teacherId) {
            for (size_t k = 0; k < size; k++) {
                if (lengthCheck == 0 || length[k] == 0) {
                    continue;
                }

                id_t endBlockId = startBlockId[k] + (id_t)length[k];
                id_t endBlockIdCheck = startBlockIdCheck + (id_t)lengthCheck;
                if (dayCheck == day[k] &&
                    startBlockIdCheck < endBlockId &&
                    endBlockIdCheck > startBlockId[k]) {
                    free(day);
                    free(startBlockId);
                    free(length);
                    return 0;
                }
            }
        }
    }

    free(day);
    free(startBlockId);
    free(length);
    return 1;
}

static int randomize_dictation(chromosome_t* chromosome,
    size_t firstPosition, size_t potentialSessionCount, size_t weeklyBlocks) {
    if (potentialSessionCount == 0) {
        return 0;
    }

    if (weeklyBlocks == 0) {
        for (size_t i = 0; i < potentialSessionCount; i++) {
            gene_t* gene = get_gene_at(chromosome, firstPosition + i);
            set_gene_day(gene, 0);
            set_gene_start_block_id(gene, 0);
            set_gene_length(gene, 0);
        }
        return 1;
    }

    size_t maxActiveSessions = potentialSessionCount;
    if (maxActiveSessions > DAY_COUNT) {
        maxActiveSessions = DAY_COUNT;
    }
    if (maxActiveSessions > weeklyBlocks) {
        maxActiveSessions = weeklyBlocks;
    }

    size_t minActiveSessions =
        (weeklyBlocks + BLOCK_COUNT - 1) / BLOCK_COUNT;

    for (size_t attempt = 0; attempt < MAX_RANDOMIZATION_ATTEMPTS; attempt++) {
        for (size_t i = 0; i < potentialSessionCount; i++) {
            gene_t* gene = get_gene_at(chromosome, firstPosition + i);
            set_gene_day(gene, 0);
            set_gene_start_block_id(gene, 0);
            set_gene_length(gene, 0);
        }

        size_t activeSessionCount = minActiveSessions +
            (size_t)rand() % (maxActiveSessions - minActiveSessions + 1);

        for (size_t i = 0; i < activeSessionCount; i++) {
            gene_t* gene = get_gene_at(chromosome, firstPosition + i);
            set_gene_length(gene, 1);
        }

        size_t remainingBlocks = weeklyBlocks - activeSessionCount;
        while (remainingBlocks > 0) {
            size_t sessionPosition = (size_t)rand() % activeSessionCount;
            gene_t* gene = get_gene_at(chromosome,
                                    firstPosition + sessionPosition);

            if (get_gene_length(gene) == BLOCK_COUNT) {
                continue;
            }

            set_gene_length(gene, get_gene_length(gene) + 1);
            remainingBlocks--;
        }

        size_t days[DAY_COUNT];
        for (size_t i = 0; i < DAY_COUNT; i++) {
            days[i] = i;
        }
        
        for (size_t i = DAY_COUNT - 1; i > 0; i--) {
            size_t other = (size_t)rand() % (i + 1);
            size_t temporary = days[i];
            days[i] = days[other];
            days[other] = temporary;
        }

        for (size_t i = 0; i < activeSessionCount; i++) {
            gene_t* gene = get_gene_at(chromosome, firstPosition + i);
            size_t length = get_gene_length(gene);

            set_gene_day(gene, days[i]);
            set_gene_start_block_id(gene, (id_t)((size_t)rand() % (BLOCK_COUNT - length + 1)));
        }

        if (validation_overlaps(chromosome, firstPosition,
                          firstPosition + potentialSessionCount)) {
            return 1;
        }
    }

    return 0;
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

int create_first_population(population_t* population) {
    if (population == NULL || population[0] == NULL) {
        return 0;
    }

    size_t geneCount = get_chromosome_gene_count(population[0]);

    for (size_t i = 1; i < POPULATION_SIZE; i++) {
        population[i] = clone_chromosome(population[0]);
        if (population[i] == NULL) {
            return 0;
        }

        size_t groupStart = 0;

        while (groupStart < geneCount) {
            gene_t* firstGroupGene = get_gene_at(population[i], groupStart);
            id_t dictationId = get_gene_dictation_id(firstGroupGene);
            size_t groupEnd = groupStart + 1;

            while (groupEnd < geneCount) {
                gene_t* nextGene = get_gene_at(population[i], groupEnd);
                if (get_gene_dictation_id(nextGene) != dictationId) {
                    break;
                }
                groupEnd++; 
            }

            dictation_t* dictation = query_dictation(dictationId);
            subject_t* subject = dictation == NULL ? NULL : query_subject(get_dictation_subject_id(dictation));

            if (subject == NULL || !randomize_dictation(population[i], groupStart,
                (groupEnd - groupStart), get_subject_weekly_hours(subject))) {
                free_chromosome(population[i]);
                population[i] = NULL;
                return 0;
            }

            groupStart = groupEnd;
        }

        set_chromosome_fitness(population[i], INFINITY);
    }

    return 1;
}

double validate_r3(population_t* population, size_t index) {
    size_t nmax = 3;
    double penalty = 0;
    if (population[index] == NULL) {
        return INFINITY;
    }
    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);
    for (size_t j = 0; j < geneCount; j++) {
        gene_t* gene = get_gene_at(chromosome, j);
        size_t length = get_gene_length(gene);
        if (length > nmax) {
            penalty += (double)(length - nmax);
        }
    }
    return penalty;
}

double validate_r5(population_t* population, size_t index) {
    if (population[index] == NULL) {
        return INFINITY;
    }
    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);    
    dynarr_t* checkedComissionId = init_dynamic_array(sizeof(id_t), 50);
    double penalty = 0.0;

    for (size_t i = 0; i < geneCount ; i++) {
        size_t dailyBlocks[DAY_COUNT] = {0};
        gene_t* gene = get_gene_at(chromosome, i);
        size_t length = get_gene_length(gene);
        size_t day = get_gene_day(gene);
        if (day >= DAY_COUNT) {
            free_dynamic_array(checkedComissionId);
            return INFINITY;
        }
        dailyBlocks[day] += length;
        id_t comissionId = get_comission_id_from_gene(gene);
        size_t checkedCount = dynamic_array_size(checkedComissionId);
        int alreadyChecked = 0;

        for (size_t j = 0; j < checkedCount; j++) {
            id_t* checkedId = (id_t*)at(checkedComissionId, j);
            if (*checkedId == comissionId) {
                alreadyChecked = 1;
                break;
            }
        }

        if (alreadyChecked) {
            continue;
        }

        id_t* newSlot = (id_t*)push_slot(checkedComissionId);
        if (newSlot == NULL) {
            free_dynamic_array(checkedComissionId);
            return INFINITY;
        }
        *newSlot = comissionId;

        for (size_t j = i+1; j < geneCount; j++) {
            gene_t* nextGene = get_gene_at(chromosome, j);
            id_t nextComissionId = get_comission_id_from_gene(nextGene);
            if (comissionId == nextComissionId) {
                size_t nextDay = get_gene_day(nextGene);
                size_t nextLength = get_gene_length(nextGene);
                if (nextDay >= DAY_COUNT) {
                    free_dynamic_array(checkedComissionId);
                    return INFINITY;
                }
                dailyBlocks[nextDay] += nextLength;
            }
        }
        size_t totalBlocks = 0;

        for (size_t day = 0; day < DAY_COUNT; day++) {
            totalBlocks += dailyBlocks[day];
        }

        double average = (double)totalBlocks / DAY_COUNT;

        for (size_t day = 0; day < DAY_COUNT; day++) {
            penalty += fabs((double)dailyBlocks[day] - average);
        }
    }
    free_dynamic_array(checkedComissionId);
    return penalty;
}

static size_t find_group_end(chromosome_t* chromosome, size_t startPosition) {
    size_t geneCount = get_chromosome_gene_count(chromosome);
    gene_t* firstGene = get_gene_at(chromosome, startPosition);
    id_t dictationId = get_gene_dictation_id(firstGene);
    size_t groupEnd = startPosition + 1;

    while (groupEnd < geneCount) {
        gene_t* nextGene = get_gene_at(chromosome, groupEnd);
        if (get_gene_dictation_id(nextGene) != dictationId) {
            break;
        }
        groupEnd++;
    }

    return groupEnd;
}

double validate_r7 (population_t* population, size_t index){
    if (population[index] == NULL) {
        return INFINITY;
    }
    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);    
    double totalPenalty = 0.0;

    for (size_t i = 0; i < geneCount; i++) {
        gene_t* gene = get_gene_at(chromosome, i);
        id_t comissionId = get_comission_id_from_gene(gene);
        size_t groupEnd = find_group_end(chromosome, i);

        for (size_t j = groupEnd; j < geneCount; j++){
            gene_t* nextGene = get_gene_at(chromosome, j);
            id_t nextComissionId = get_comission_id_from_gene(nextGene);
            size_t nextGroupEnd = find_group_end(chromosome, j);

            for (size_t k = i; k < groupEnd; k++){
                gene_t* currentGene = get_gene_at(chromosome, k);
                size_t currentDay = get_gene_day(currentGene);
                size_t currentStartBlock = get_gene_start_block_id(currentGene);
                size_t currentLength = get_gene_length(currentGene);
                if (currentLength == 0) {
                    continue;
                }

                for (size_t l = j; l < nextGroupEnd; l++){
                    gene_t* nextGeneInGroup = get_gene_at(chromosome, l);
                    size_t nextDay = get_gene_day(nextGeneInGroup);
                    size_t nextStartBlock = get_gene_start_block_id(nextGeneInGroup);
                    size_t nextLength = get_gene_length(nextGeneInGroup);
                    if (nextLength == 0) {
                        continue;
                    }

                    if (currentDay == nextDay && comissionId != nextComissionId) {
                        size_t currentEndBlock = currentStartBlock + currentLength;
                        size_t nextEndBlock = nextStartBlock + nextLength;
                        size_t overlapStart = currentStartBlock > nextStartBlock
                            ? currentStartBlock
                            : nextStartBlock;
                        size_t overlapEnd = currentEndBlock < nextEndBlock
                            ? currentEndBlock
                            : nextEndBlock;

                        if (overlapStart >= overlapEnd) {
                            continue;
                        }

                        size_t overlappingBlocks = overlapEnd - overlapStart;
                        id_t subjectId = get_subject_id_from_gene(currentGene);
                        id_t nextSubjectId = get_subject_id_from_gene(nextGeneInGroup); 
                        totalPenalty += r7_conflict_weight(subjectId, nextSubjectId)
                            * (double)overlappingBlocks;
                    }
                }
            }

            j = nextGroupEnd - 1;
        }

        
        i = groupEnd - 1;
    }

    return totalPenalty;
}
