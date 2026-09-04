#include <stdio.h>
#include <stdint.h>
#include "load/schedule_loader.h"

int main() {
    schedule_file_t file = open_file("C:/Users/PC/Downloads/horarios2do.xlsx");

    size_t com_count;
    com_id_t* comission_ids;
    get_comissions(file, &com_count, &comission_ids);

    schedule_data_t* sch;
    get_schedule_for_comission(file, "2k01", &sch);
    close_file(file);
    return 0;
}