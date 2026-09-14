#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "load/subjectLoader.h"
#include "chromosomeLoader.h"
#include "algGen.h"
#include "db.h"

#define ROLL_COUNT 300

int main(void) {
    srand(SEED);
    init_db();

    load_subjects("./in/plan2023.csv");

    chromosome_t* firstChromosome = load_chromsome_from_files("./in/");
    if (firstChromosome == NULL) {
        fprintf(stderr, "No se pudo cargar el horario inicial.\n");
        terminate_db();
        return EXIT_FAILURE;
    }

    population_t* population = init_population(firstChromosome);
    free_chromosome(firstChromosome);

    if (population == NULL || !create_first_population(population)) {
        fprintf(stderr, "No se pudo generar la poblacion inicial.\n");
        free_population(population);
        terminate_db();
        return EXIT_FAILURE;
    }

    for (size_t r = 0; r < ROLL_COUNT; r++) {
        for(size_t i = 0; i < POPULATION_SIZE; i++) {
            set_chromosome_fitness(population[i], fitness_function(population, i));
        }

        population_t* newPopulation = calloc(POPULATION_SIZE, sizeof(*newPopulation));
        if (newPopulation == NULL) {
            fprintf(stderr, "No se pudo reservar memoria para la nueva poblacion.\n");
            free_population(population);
            terminate_db();
            return EXIT_FAILURE;
        }

        for(size_t i = 0; i < POPULATION_SIZE; i+=2) {
            chromosome_t* selected1 = tournament(population);
            chromosome_t* selected2 = tournament(population);
            newPopulation[i] = clone_chromosome(selected1);
            newPopulation[i+1] = clone_chromosome(selected2);
            if (newPopulation[i] == NULL || newPopulation[i+1] == NULL) {
                fprintf(stderr, "No se pudieron clonar los padres seleccionados.\n");
                free_population(newPopulation);
                free_population(population);
                terminate_db();
                return EXIT_FAILURE;
            }

            crossover_chromosomes(newPopulation[i], newPopulation[i+1]);
            mutation_chromosome(newPopulation[i]);
            mutation_chromosome(newPopulation[i+1]);
        }

        free_population(population);
        population = newPopulation;
    }

    double bestFitness = INFINITY;
    for (size_t i = 0; i < POPULATION_SIZE; i++) {
        double fitness = fitness_function(population, i);
        set_chromosome_fitness(population[i], fitness);
        if (fitness < bestFitness) bestFitness = fitness;
    }
    printf("Mejor fitness de la ultima generacion: %.6f\n", bestFitness);
    free_population(population);

    terminate_db();
    return EXIT_SUCCESS;
}
