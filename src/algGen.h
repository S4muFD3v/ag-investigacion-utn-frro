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
#define CROSSOVER_PROBABILITY 0.7
#define MUTATION_PROBABILITY 0.05
#define TOURNAMENT_SIZE 3
#define R3_VALUE 1
#define R5_VALUE 1
#define R6_VALUE 1
#define R7_VALUE 1
#define R8_VALUE 1
#define R12_VALUE 1

typedef chromosome_t* population_t;

population_t *init_population(const chromosome_t *initial);
void free_population(population_t *population);
int create_first_population(population_t *population);

double fitness_function(population_t *population, size_t index);
void crossover_chromosomes(chromosome_t* chromosome1, chromosome_t* chromosome2);
void mutation_chromosome(chromosome_t* chromosome);

chromosome_t* tournament(population_t* population);

#endif
