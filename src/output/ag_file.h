#ifndef AG_FILE_H
#define AG_FILE_H

#include "../chromosome.h"

/* Crea <filePrefix>_anio_<N>.csv con las grillas del cromosoma. */
int export_timetable_by_year(chromosome_t* chromosome, const char* filePrefix);

void init(const char* fileName);
void init_roll(size_t roll);

void write_chromosome(const chromosome_t* c, size_t id);
void write_roll_data(
    double bestFitness, double worstFitness, double averageFitness, double stdDev);
void terminate();
#endif
