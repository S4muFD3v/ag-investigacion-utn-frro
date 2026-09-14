#include "ag_file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../algGen.h"
#include "../db.h"

typedef struct {
    id_t comissionId;
    subject_t* subject;
    fmp_t fmp;
    size_t day;
    id_t startBlock;
    size_t length;
} timetable_entry_t;

static void write_csv_text(FILE* file, const char* text) {
    for (; *text != '\0'; text++) {
        if (*text == '"') fputc('"', file);
        fputc((unsigned char)*text, file);
    }
}

static const char* schedule_name(schedule_t schedule) {
    switch (schedule) {
        case SCHEDULE_MORNING: return "Manana";
        case SCHEDULE_AFTERNOON: return "Tarde";
        case SCHEDULE_EVENING: return "Noche";
        default: return "Sin turno";
    }
}

static id_t schedule_block_offset(schedule_t schedule) {
    switch (schedule) {
        case SCHEDULE_MORNING: return 0;
        case SCHEDULE_AFTERNOON: return BLOCK_COUNT - 1;
        case SCHEDULE_EVENING: return 2 * (BLOCK_COUNT - 1);
        default: return -1;
    }
}

static void write_timetable_cell(FILE* file, const timetable_entry_t* entries,
    size_t entryCount, id_t comissionId, size_t period, size_t day, size_t block) {
    fputc('"', file);
    int first = 1;
    for (size_t i = 0; i < entryCount; i++) {
        const timetable_entry_t* entry = &entries[i];
        int active = entry->fmp == FMP_BOTH ||
            (period == 0 && entry->fmp == FMP_FIRST) ||
            (period == 1 && entry->fmp == FMP_SECOND);
        if (entry->comissionId != comissionId || !active || entry->day != day ||
            (id_t)block < entry->startBlock ||
            (size_t)((id_t)block - entry->startBlock) >= entry->length) {
            continue;
        }

        // Si hubiera un solapamiento, se muestran ambas materias en la celda.
        if (!first) fputs(" | ", file);
        write_csv_text(file, get_subject_name(entry->subject));
        first = 0;
    }
    fputc('"', file);
}

static int write_comission_timetable(FILE* file, comission_t* comission,
    const timetable_entry_t* entries, size_t entryCount) {
    id_t comissionId = get_comission_id(comission);
    schedule_t schedule = get_comission_schedule(comission);
    id_t offset = schedule_block_offset(schedule);
    if (offset < 0) return 0;

    for (size_t period = 0; period < ACADEMIC_PERIOD_COUNT; period++) {
        for (size_t block = 0; block < BLOCK_COUNT; block++) {
            fprintf(file, "%zu;\"", get_comission_year(comission));
            const char* name = get_comission_name(comission);
            if (name[0] != '\0') write_csv_text(file, name);
            else fprintf(file, "%lld", comissionId);
            fprintf(file, "\";%lld;%s;%zu;%zu;%lld", comissionId,
                schedule_name(schedule), period + 1, block, offset + (id_t)block);
            for (size_t day = 0; day < DAY_COUNT; day++) {
                fputc(';', file);
                write_timetable_cell(file, entries, entryCount, comissionId,
                    period, day, block);
            }
            fputs("\r\n", file);
        }
    }
    return !ferror(file);
}

int export_timetable_by_year(chromosome_t* chromosome, const char* filePrefix) {
    if (chromosome == NULL || filePrefix == NULL) return 0;
    size_t geneCount = get_chromosome_gene_count(chromosome);
    if (geneCount == 0) return 0;
    timetable_entry_t* entries = malloc(geneCount * sizeof(*entries));
    if (entries == NULL) return 0;

    size_t entryCount = 0;
    // Resolvemos los datos una sola vez; las grillas no vuelven a consultar la DB.
    for (size_t i = 0; i < geneCount; i++) {
        gene_t* gene = get_gene_at(chromosome, i);
        if (get_gene_length(gene) == 0) continue;
        dictation_t* dictation = query_dictation(get_gene_dictation_id(gene));
        subject_t* subject = dictation == NULL ? NULL :
            query_subject(get_dictation_subject_id(dictation));
        fmp_t fmp = get_fmp_from_gene(gene);
        if (subject == NULL || (fmp != FMP_FIRST && fmp != FMP_SECOND &&
                fmp != FMP_BOTH)) {
            fprintf(stderr, "No se pudieron resolver los datos del gen %zu para exportar.\n", i);
            free(entries);
            return 0;
        }
        entries[entryCount++] = (timetable_entry_t){
            get_dictation_comission_id(dictation), subject, fmp,
            get_gene_day(gene), get_gene_start_block_id(gene), get_gene_length(gene)
        };
    }

    size_t filenameSize = strlen(filePrefix) + 64;
    char* filename = malloc(filenameSize);
    if (filename == NULL) {
        free(entries);
        return 0;
    }
    int success = 1;
    size_t comissionCount = get_comission_count();
    for (size_t i = 0; i < comissionCount && success; i++) {
        size_t year = get_comission_year(get_comission_at(i));
        int alreadyExported = 0;
        for (size_t j = 0; j < i; j++) {
            if (get_comission_year(get_comission_at(j)) == year) {
                alreadyExported = 1;
                break;
            }
        }
        if (alreadyExported) continue;

        snprintf(filename, filenameSize, "%s_anio_%zu.csv", filePrefix, year);
        FILE* file = fopen(filename, "wb");
        if (file == NULL) {
            fprintf(stderr, "No se pudo crear el CSV de horarios '%s'.\n", filename);
            success = 0;
            break;
        }
        // BOM UTF-8 para que Excel reconozca los acentos de las materias.
        fputs("\xef\xbb\xbf", file);
        fputs("Anio;Comision;ComisionId;Turno;Cuatrimestre;BloqueLocal;BloqueGlobal;"
            "Lunes;Martes;Miercoles;Jueves;Viernes\r\n", file);
        for (size_t j = 0; j < comissionCount && success; j++) {
            comission_t* comission = get_comission_at(j);
            if (get_comission_year(comission) == year) {
                success = write_comission_timetable(file, comission, entries, entryCount);
            }
        }
        if (fclose(file) != 0) success = 0;
        if (success) printf("Horario del anio %zu: %s\n", year, filename);
        else fprintf(stderr, "No se pudo completar el CSV de horarios '%s'.\n", filename);
    }
    free(filename);
    free(entries);
    return success;
}

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
