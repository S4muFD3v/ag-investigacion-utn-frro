#include "ag_file.h"
#include <stdio.h>
#include <string.h>

#ifndef C_GENE_COUNT
    #define C_GENE_COUNT 0
    //#error "Importante definir la cantidad de genes"
#endif

typedef struct global_file_t {
    const char* extension;
    const char* fileName;
    FILE* populationDataFile;
    FILE* rollDataFile;
    size_t currentRoll;
} global_file_t;

global_file_t gf;


void init(const char* fileName) {
    gf.fileName = fileName;
    gf.extension = "csv";
    gf.currentRoll = 0;
    gf.populationDataFile = NULL;
    const size_t fileNameSize = strlen(gf.fileName);
    const size_t fileExtensionSize = strlen(gf.extension);
    const size_t fullNameSize = fileNameSize + fileExtensionSize + 2;

    char fileFullName[255];
    memset(fileFullName, '\0', 255);
    snprintf(fileFullName, 255, "%s.%s", gf.fileName, gf.extension);
    
    printf("Abriendo archivo de poblaciones\n");
    if((gf.populationDataFile = fopen(fileFullName, "w")) == NULL) {
        printf("Error al crear el archivo de poblaciones\n");
        exit(1);
    }
    if(fseek(gf.populationDataFile, 0, SEEK_SET) != 0) {
        printf("Error al posicionar el puntero en el archivo de poblaciones\n");
        exit(1);
    }
    
    memset(fileFullName, '\0', 255);
    snprintf(fileFullName, 255, "%s.resumen.%s", gf.fileName, gf.extension);
    printf("Abriendo archivo de tiradas\n");
    if((gf.rollDataFile = fopen(fileFullName, "w")) == NULL) {
        printf("Error al crear el archivo de tiradas\n");
        exit(1);
    }
    if(fseek(gf.rollDataFile, 0, SEEK_SET) != 0) {
        printf("Error al posicionar el puntero en el archivo de tiradas\n");
        exit(1);
    }

    printf("\nArchivos abiertos\n");
    fprintf(gf.populationDataFile, "Tirada;Cromosoma;Valor genes;Fitness\n");
    fprintf(gf.rollDataFile, "Tirada;Mejor;Peor;Promedio;Desviación estandar\n");
}


void init_roll(size_t roll) {
    gf.currentRoll = roll;
    printf("\nEscribiendo tirada %lu\n", roll);
}


void write_chromosome(const chromosome_t* c, size_t id) {
    return;
}


void write_roll_data(
    double bestFitness, double worstFitness, double averageFitness, double stdDev) {
    fprintf(gf.rollDataFile, "%lu;%.6f;%.6f;%.6f;%.6f\n", gf.currentRoll, 
        bestFitness, worstFitness, averageFitness, stdDev);
    printf( 
        "Resumen de tirada %lu: Mejor %.6f Peor %.6f Promedio %.6f Desviación estandar %.6f\n", 
        gf.currentRoll, bestFitness, worstFitness, averageFitness, stdDev);
}


void terminate() {
    printf("\nCerrando arhivos\n");
    if(fclose(gf.populationDataFile) != 0) {
        printf("Error al cerrar el archivo\n");
        return;
    }
    if(fclose(gf.rollDataFile) != 0) {
        printf("Error al cerrar el archivo\n");
        return;
    }
    printf("Arhivos cerrados\n");
}
