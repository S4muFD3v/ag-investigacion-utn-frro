#ifndef FITNESS_FILE_H
#define FITNESS_FILE_H

#include "../algGen.h"

int open_fitness_file(const char* filename, unsigned int seed, double initialFitness);
int write_fitness_roll(population_t* population, size_t roll);
int close_fitness_file(void);

#endif
