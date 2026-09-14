#ifndef AG_FILE_H
#define AG_FILE_H

#include "../chromosome.h"

void init(const char* fileName);
void init_roll(size_t roll);

void write_chromosome(const chromosome_t* c, size_t id);
void write_roll_data(
    double bestFitness, double worstFitness, double averageFitness, double stdDev);
void terminate();
#endif