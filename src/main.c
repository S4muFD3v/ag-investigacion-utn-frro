#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "load/schedule_loader.h"
#include "load/subjectLoader.h"
#include "chromosomeLoader.h"
#include "algGen.h"
#include "db.h"

int main() {
    srand(SEED);
    init_db();

    load_subjects("./in/subjects.csv");

    schedule_file_t file = open_file("C:/Users/PC/Downloads/horarios2do.xlsx");

    chromosome_t* firstChromosome = load_chromsome_from_files("./in/");

    subject_t* ss = get_subject_at(0);
    comission_t* coms = get_comission_at(0);
    dictation_t* dicts = get_dictation_at(0);

    terminate_db();
    return 0;
}
