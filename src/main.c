#include <stdio.h>
#include "load/schedule_loader.h"

int main() {
    printf("Clasik");
    schedule_file_t file = open_file("C:/Users/PC/Downloads/horarios2do.xlsx");

    size_t com_count;
    size_t* comission_ids;
    get_comissions(file, com_count, comission_ids);
    close_file(file);
    return 0;
}