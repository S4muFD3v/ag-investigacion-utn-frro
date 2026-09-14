#include "fitness_file.h"
#include <stdio.h>
#include <math.h>

static FILE* fitnessFile;
static unsigned int runSeed;
static double originalFitness;
static double bestHistoricalFitness;

int open_fitness_file(const char* filename, unsigned int seed, double initialFitness) {
    fitnessFile = fopen(filename, "w");
    if (fitnessFile == NULL) return 0;

    runSeed = seed;
    originalFitness = initialFitness;
    bestHistoricalFitness = INFINITY;
    if (fprintf(fitnessFile,
            "Roll;Seed;FitnessInicial;Mejor;Promedio;Peor;MejorHistorico\n") < 0 ||
        fflush(fitnessFile) != 0) {
        close_fitness_file();
        return 0;
    }
    return 1;
}

int write_fitness_roll(population_t* population, size_t roll) {
    double bestFitness = get_chromosome_fitness(population[0]);
    double worstFitness = bestFitness;
    double sumFitness = 0.0;

    for (size_t i = 0; i < POPULATION_SIZE; i++) {
        double fitness = get_chromosome_fitness(population[i]);
        if (fitness < bestFitness) bestFitness = fitness;
        if (fitness > worstFitness) worstFitness = fitness;
        sumFitness += fitness;
    }
    if (bestFitness < bestHistoricalFitness) {
        bestHistoricalFitness = bestFitness;
    }

    return fprintf(fitnessFile, "%zu;%u;%.6f;%.6f;%.6f;%.6f;%.6f\n",
        roll, runSeed, originalFitness, bestFitness, sumFitness / POPULATION_SIZE,
        worstFitness, bestHistoricalFitness) >= 0 && fflush(fitnessFile) == 0;
}

int close_fitness_file(void) {
    if (fitnessFile == NULL) return 1;
    int result = fclose(fitnessFile);
    fitnessFile = NULL;
    return result == 0;
}
