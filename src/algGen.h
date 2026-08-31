#ifndef _ALGEN_H_
#define _ALGEN_H_

#include "chromosome.h"
#define POPULATION_SIZE 30


typedef chromosome_t* population_t;

population_t *init_population(const chromosome_t *initial);
void free_population(population_t *population);
void create_first_population(population_t *population);

#endif