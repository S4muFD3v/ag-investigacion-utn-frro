#include "algGen.h"
#include "chromosome.h"
#include <stdlib.h>
#include "db.h"
#include "utils/memory.h"
#include "utils/r7_matrix.h"
#include <math.h>


typedef struct {
    id_t comissionId;
    schedule_t schedule;
    size_t year;
    size_t firstStart[ACADEMIC_PERIOD_COUNT][DAY_COUNT];
    size_t lastEnd[ACADEMIC_PERIOD_COUNT][DAY_COUNT];
    int isOccupied[ACADEMIC_PERIOD_COUNT][DAY_COUNT];
} comission_schedule_t;

static int share_academic_period(fmp_t first, fmp_t second) {
    return first == FMP_BOTH || second == FMP_BOTH || first == second;
}

static int is_active_in_period(fmp_t fmp, size_t period) {
    return fmp == FMP_BOTH ||
           (period == 0 && fmp == FMP_FIRST) ||
           (period == 1 && fmp == FMP_SECOND);
}


static id_t local_to_global_block(id_t comissionId, id_t localBlock){

    comission_t* comission = query_comission(comissionId);
    if (comission == NULL) {
        return -1;
    }

    schedule_t schedule = get_comission_schedule(comission);

    switch (schedule) {
        case SCHEDULE_MORNING:
            return localBlock;

        case SCHEDULE_AFTERNOON:
            return localBlock + 7;

        case SCHEDULE_EVENING:
            return localBlock + 14;

        default:
            return -1;
    }
}

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

    gene_t* firstGene = get_gene_at(chromosome, firstPosition);
    id_t comissionId = get_comission_id_from_gene(firstGene);
    id_t teacherId = get_teacher_id_from_gene(firstGene);
    fmp_t fmp = get_fmp_from_gene(firstGene);

    for (size_t i = firstPosition; i < lastPosition; i++) {
        gene_t* gene = get_gene_at(chromosome, i);
        day[i - firstPosition] = get_gene_day(gene);
        id_t startBlockIdValue = get_gene_start_block_id(gene);
        startBlockId[i - firstPosition] = local_to_global_block(comissionId, startBlockIdValue);
        length[i - firstPosition] = get_gene_length(gene);   
    }

    for (size_t j = 0; j < firstPosition; j++) {
        gene_t* geneCheck = get_gene_at(chromosome, j);
        size_t dayCheck = get_gene_day(geneCheck);
        id_t comissionIdCheck = get_comission_id_from_gene(geneCheck);
        id_t startBlockIdCheck = local_to_global_block(comissionIdCheck, get_gene_start_block_id(geneCheck));
        size_t lengthCheck = get_gene_length(geneCheck);
        id_t teacherIdCheck = get_teacher_id_from_gene(geneCheck);
        fmp_t fmpCheck = get_fmp_from_gene(geneCheck);
        
        if ((comissionIdCheck == comissionId || teacherIdCheck == teacherId) &&
            share_academic_period(fmp, fmpCheck)) {
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
    double penalty = 0;
    if (population[index] == NULL) {
        return INFINITY;
    }
    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);
    for (size_t j = 0; j < geneCount; j++) {
        gene_t* gene = get_gene_at(chromosome, j);
        size_t length = get_gene_length(gene);
        if (length > R3_MAX_LENGTH) {
            penalty += (double)(length - R3_MAX_LENGTH);
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
        gene_t* gene = get_gene_at(chromosome, i);
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

        size_t dailyBlocks[ACADEMIC_PERIOD_COUNT][DAY_COUNT] = {{0}};

        for (size_t j = i; j < geneCount; j++) {
            gene_t* comissionGene = get_gene_at(chromosome, j);
            if (get_comission_id_from_gene(comissionGene) != comissionId) {
                continue;
            }

            size_t day = get_gene_day(comissionGene);
            size_t length = get_gene_length(comissionGene);
            fmp_t fmp = get_fmp_from_gene(comissionGene);

            for (size_t period = 0; period < ACADEMIC_PERIOD_COUNT; period++) {
                if (is_active_in_period(fmp, period)) {
                    dailyBlocks[period][day] += length;
                }
            }
        }

        for (size_t period = 0; period < ACADEMIC_PERIOD_COUNT; period++) {
            size_t totalBlocks = 0;

            for (size_t day = 0; day < DAY_COUNT; day++) {
                totalBlocks += dailyBlocks[period][day];
            }

            double average = (double)totalBlocks / DAY_COUNT;

            for (size_t day = 0; day < DAY_COUNT; day++) {
                penalty += fabs((double)dailyBlocks[period][day] - average);
            }
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
        fmp_t fmp = get_fmp_from_gene(gene);
        size_t groupEnd = find_group_end(chromosome, i);

        for (size_t j = groupEnd; j < geneCount; j++){
            gene_t* nextGene = get_gene_at(chromosome, j);
            id_t nextComissionId = get_comission_id_from_gene(nextGene);
            fmp_t nextFmp = get_fmp_from_gene(nextGene);
            size_t nextGroupEnd = find_group_end(chromosome, j);

            if (!share_academic_period(fmp, nextFmp)) {
                j = nextGroupEnd - 1;
                continue;
            }

            for (size_t k = i; k < groupEnd; k++){
                gene_t* currentGene = get_gene_at(chromosome, k);
                size_t currentDay = get_gene_day(currentGene);
                size_t currentStartBlock = local_to_global_block(comissionId, get_gene_start_block_id(currentGene));
                size_t currentLength = get_gene_length(currentGene);
                if (currentLength == 0) {
                    continue;
                }

                for (size_t l = j; l < nextGroupEnd; l++){
                    gene_t* nextGeneInGroup = get_gene_at(chromosome, l);
                    size_t nextDay = get_gene_day(nextGeneInGroup);
                    size_t nextStartBlock = local_to_global_block(nextComissionId, get_gene_start_block_id(nextGeneInGroup));
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

static int are_consecutive_blocks (schedule_t firstSchedule, schedule_t secondSchedule) {
    return (firstSchedule == SCHEDULE_MORNING && secondSchedule == SCHEDULE_AFTERNOON) ||
           (firstSchedule == SCHEDULE_AFTERNOON && secondSchedule == SCHEDULE_EVENING);
}

double validate_r6(population_t* population, size_t index) {
    if (population[index] == NULL) {
        return INFINITY;
    }

    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);
    size_t comissionCount = get_comission_count();
    size_t usedComissions = 0;
    double totalPenalty = 0.0;
    comission_schedule_t* comissionSchedules =
        (comission_schedule_t*)calloc(comissionCount, sizeof(comission_schedule_t));

    if (comissionSchedules == NULL) {
        return INFINITY;
    }

    for (size_t i = 0; i < geneCount; i++) {
        gene_t* gene = get_gene_at(chromosome, i);
        size_t length = get_gene_length(gene);
        if (length == 0) {
            continue;
        }

        id_t comissionId = get_comission_id_from_gene(gene);
        comission_t* comission = query_comission(comissionId);
        size_t day = get_gene_day(gene);
        schedule_t schedule = get_comission_schedule(comission);
        size_t initialBlock = (size_t)local_to_global_block(
            comissionId, get_gene_start_block_id(gene));
        size_t finalBlock = initialBlock + length - 1;
        fmp_t fmp = get_fmp_from_gene(gene);
        size_t position = 0;

        while (position < usedComissions && comissionSchedules[position].comissionId != comissionId) {
            position++;
        }

        if (position == usedComissions) {
            comissionSchedules[position].comissionId = comissionId;
            comissionSchedules[position].schedule = schedule;
            comissionSchedules[position].year = get_comission_year(comission);
            usedComissions++;
        }

        for (size_t period = 0; period < ACADEMIC_PERIOD_COUNT; period++) {
            if (!is_active_in_period(fmp, period)) {
                continue;
            }

            if (!comissionSchedules[position].isOccupied[period][day]) {
                comissionSchedules[position].firstStart[period][day] = initialBlock;
                comissionSchedules[position].lastEnd[period][day] = finalBlock;
                comissionSchedules[position].isOccupied[period][day] = 1;
            } else {
                if (initialBlock < comissionSchedules[position].firstStart[period][day]) {
                    comissionSchedules[position].firstStart[period][day] = initialBlock;
                }
                if (finalBlock > comissionSchedules[position].lastEnd[period][day]) {
                    comissionSchedules[position].lastEnd[period][day] = finalBlock;
                }
            }
        }
    }

    for (size_t i = 0; i < usedComissions; i++) {
        for (size_t j = 0; j < usedComissions; j++) {
            if (!are_consecutive_blocks(comissionSchedules[i].schedule,
                                        comissionSchedules[j].schedule)) {
                continue;
            }

            size_t yearDifference =
                comissionSchedules[i].year > comissionSchedules[j].year
                    ? comissionSchedules[i].year - comissionSchedules[j].year
                    : comissionSchedules[j].year - comissionSchedules[i].year;
            double yearWeight = 1.0 / ((double)yearDifference + 1.0);

            for (size_t period = 0; period < ACADEMIC_PERIOD_COUNT; period++) {
                for (size_t day = 0; day < DAY_COUNT; day++) {
                    if (!comissionSchedules[i].isOccupied[period][day] ||
                        !comissionSchedules[j].isOccupied[period][day]) {
                        continue;
                    }

                    size_t previousEnd =
                        comissionSchedules[i].lastEnd[period][day];
                    size_t nextStart =
                        comissionSchedules[j].firstStart[period][day];

                    if (nextStart > previousEnd + R6_MAX_BLOCK_DIFFERENCE) {
                        size_t excessBlocks =
                            nextStart - previousEnd - R6_MAX_BLOCK_DIFFERENCE;
                        totalPenalty += (double)excessBlocks * yearWeight;
                    }
                }
            }
        }
    }

    free(comissionSchedules);
    return totalPenalty;
}

static int is_elective_subject(id_t subjectId) {
    const r7_subject_info_t* subjectInfo = r7_subject_by_id(subjectId);
    if (subjectInfo == NULL) {
        return 0;
    }
    return subjectInfo->kind == R7_SUBJECT_ELECTIVE;
}

double validate_r8 (population_t* population, size_t index){
    if (population[index] == NULL) {
        return INFINITY;
    }

    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);
    double totalPenalty = 0.0;

    for (size_t i = 0; i < geneCount; i++) {
        gene_t* gene = get_gene_at(chromosome, i);
        id_t subjectId = get_subject_id_from_gene(gene);
        if (!is_elective_subject(subjectId)) continue;

        id_t dictationId = get_gene_dictation_id(gene);
        size_t n = 1;
        while ((i + n) < geneCount &&
               dictationId == get_gene_dictation_id(get_gene_at(chromosome, i + n))) {
            n++;
        }

        id_t* comissionChecked =
            (id_t*)calloc(get_comission_count(), sizeof(id_t));
        size_t* day = (size_t*)calloc(n, sizeof(size_t));
        size_t* startBlock = (size_t*)calloc(n, sizeof(size_t));
        size_t* length = (size_t*)calloc(n, sizeof(size_t));

        if (comissionChecked == NULL || day == NULL ||
            startBlock == NULL || length == NULL) {
            free(comissionChecked);
            free(day);
            free(startBlock);
            free(length);
            return INFINITY;
        }

        size_t comissionCheckedCount = 0;
        fmp_t fmp = get_fmp_from_gene(gene);
        const r7_subject_info_t* electiveInfo = r7_subject_by_id(subjectId);

        for (size_t j = 0; j < n; j++) {
            gene_t* currentGene = get_gene_at(chromosome, i + j);
            day[j] = get_gene_day(currentGene);
            startBlock[j] = local_to_global_block(get_comission_id_from_gene(currentGene), get_gene_start_block_id(currentGene));
            length[j] = get_gene_length(currentGene);
        }

        for (size_t j = 0; j < geneCount; j++) {
            gene_t* geneCheck = get_gene_at(chromosome, j);
            id_t comissionIdCheck = get_comission_id_from_gene(geneCheck);
            int alreadyChecked = 0;

            for (size_t k = 0; k < comissionCheckedCount; k++) {
                if (comissionChecked[k] == comissionIdCheck) {
                    alreadyChecked = 1;
                    break;
                }
            }

            if (is_elective_subject(get_subject_id_from_gene(geneCheck)) ||
                alreadyChecked) {
                continue;
            }

            size_t dayCheck = get_gene_day(geneCheck);
            size_t startBlockCheck = local_to_global_block(
                comissionIdCheck, get_gene_start_block_id(geneCheck));
            size_t lengthCheck = get_gene_length(geneCheck);

            if (lengthCheck == 0 ||
                !share_academic_period(fmp, get_fmp_from_gene(geneCheck))) {
                continue;
            }

            for (size_t k = 0; k < n; k++) {
                if (length[k] == 0) {
                    continue;
                }

                if (day[k] == dayCheck &&
                    startBlock[k] < startBlockCheck + lengthCheck &&
                    startBlock[k] + length[k] > startBlockCheck) {
                    comission_t* comission = query_comission(comissionIdCheck);
                    size_t comissionYear = get_comission_year(comission);
                    size_t yearDifference = comissionYear > electiveInfo->year
                        ? comissionYear - electiveInfo->year
                        : electiveInfo->year - comissionYear;
                    double yearWeight =
                        1.0 / ((double)yearDifference + 1.0);

                    totalPenalty += yearWeight;
                    comissionChecked[comissionCheckedCount] = comissionIdCheck;
                    comissionCheckedCount++;
                    break;
                }
            }
        }

        free(comissionChecked);
        free(day);
        free(startBlock);
        free(length);
        i = i + n - 1;
    }
    return totalPenalty;
}


double validate_r12(population_t* population, size_t index) {
    if (population[index] == NULL) {
        return INFINITY;
    }

    chromosome_t* chromosome = population[index];
    size_t geneCount = get_chromosome_gene_count(chromosome);
    double totalPenalty = 0.0;

    for (size_t i = 0; i < geneCount; i++) {
        size_t groupEnd = find_group_end(chromosome, i);
        size_t occupiedDays[DAY_COUNT] = {0};

        for (size_t j = i; j < groupEnd; j++) {
            gene_t* gene = get_gene_at(chromosome, j);
            if (get_gene_length(gene) > 0) {
                occupiedDays[get_gene_day(gene)] = 1;
            }
        }

        for (size_t day = 0; day < DAY_COUNT - 1; day++) {
            if (occupiedDays[day] && occupiedDays[day + 1]) {
                totalPenalty += 1.0;
            }
        }

        i = groupEnd - 1;
    }

    return totalPenalty;
}
