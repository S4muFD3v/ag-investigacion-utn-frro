#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "load/subjectLoader.h"
#include "chromosomeLoader.h"
#include "algGen.h"
#include "db.h"
#include "output/fitness_file.h"
#include "output/ag_file.h"

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

    for (size_t i = 0; i < POPULATION_SIZE; i++) {
        set_chromosome_fitness(population[i], fitness_function(population, i));
    }
    double firstFitness = get_chromosome_fitness(population[0]);

    char fitnessFilename[160];
    time_t now = time(NULL);
    char timestamp[32];
    struct tm* runTime = localtime(&now);
    if (runTime == NULL || strftime(timestamp, sizeof(timestamp),
            "%Y%m%d_%H%M%S", runTime) == 0) {
        fprintf(stderr, "No se pudo obtener la fecha para el CSV del fitness.\n");
        free_population(population);
        terminate_db();
        return EXIT_FAILURE;
    }
    snprintf(fitnessFilename, sizeof(fitnessFilename),
        "./output/fitness_%s_seed_%u.csv", timestamp, (unsigned int)SEED);
    if (!open_fitness_file(fitnessFilename, (unsigned int)SEED, firstFitness) ||
        !write_fitness_roll(population, 0)) {
        fprintf(stderr, "No se pudo crear el CSV del fitness '%s'.\n", fitnessFilename);
        close_fitness_file();
        free_population(population);
        terminate_db();
        return EXIT_FAILURE;
    }
    printf("Evolucion del fitness: %s\n", fitnessFilename);

    for (size_t r = 0; r < ROLL_COUNT; r++) {
        population_t* newPopulation = calloc(POPULATION_SIZE, sizeof(*newPopulation));
        if (newPopulation == NULL) {
            fprintf(stderr, "No se pudo reservar memoria para la nueva poblacion.\n");
            close_fitness_file();
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
                close_fitness_file();
                free_population(newPopulation);
                free_population(population);
                terminate_db();
                return EXIT_FAILURE;
            }

            crossover_chromosomes(newPopulation[i], newPopulation[i+1]);
            mutation_chromosome(newPopulation[i]);
            mutation_chromosome(newPopulation[i+1]);
        }

        for (size_t i = 0; i < POPULATION_SIZE; i++) {
            set_chromosome_fitness(newPopulation[i], fitness_function(newPopulation, i));
        }
        free_population(population);
        population = newPopulation;
        if (!write_fitness_roll(population, r + 1)) {
            fprintf(stderr, "No se pudo escribir el roll %zu en el CSV del fitness.\n", r + 1);
            close_fitness_file();
            free_population(population);
            terminate_db();
            return EXIT_FAILURE;
        }
    }

    size_t bestIndex = 0;
    double bestFitness = get_chromosome_fitness(population[0]);
    for (size_t i = 1; i < POPULATION_SIZE; i++) {
        double fitness = get_chromosome_fitness(population[i]);
        if (fitness < bestFitness) {
            bestFitness = fitness;
            bestIndex = i;
        }
    }
    printf("Mejor fitness de la ultima generacion: %.6f\n primer fitness: %.6f\n", bestFitness, firstFitness);
    int csvClosed = close_fitness_file();
    char timetablePrefix[160];
    snprintf(timetablePrefix, sizeof(timetablePrefix),
        "./output/horarios_%s_seed_%u", timestamp, (unsigned int)SEED);
    int timetableExported = export_timetable_by_year(population[bestIndex], timetablePrefix);
    free_population(population);

    terminate_db();
    if (!csvClosed) {
        fprintf(stderr, "No se pudo cerrar correctamente el CSV del fitness.\n");
        return EXIT_FAILURE;
    }
    if (!timetableExported) {
        fprintf(stderr, "No se pudieron exportar todos los horarios.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
