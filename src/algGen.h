#ifndef _ALGEN_H_
#define _ALGEN_H_

#include "chromosome.h"
#define POPULATION_SIZE 30
#define DAY_COUNT 5
#define BLOCK_COUNT 8
#define MAX_RANDOMIZATION_ATTEMPTS 100
#define R3_MAX_LENGTH 3
#define R6_MAX_BLOCK_DIFFERENCE 1
#define ACADEMIC_PERIOD_COUNT 2

typedef chromosome_t* population_t;

population_t *init_population(const chromosome_t *initial);
void free_population(population_t *population);
int create_first_population(population_t *population);
double validate_r3(population_t* population, size_t index);
double validate_r5(population_t* population, size_t index);
double validate_r6(population_t* population, size_t index);
double validate_r7(population_t* population, size_t index);
double validate_r8(population_t* population, size_t index);
double validate_r12(population_t* population, size_t index);

#endif
