#include "schedule_loader.h"
#define BUILD_XLSXIO_STATIC
#define BUILD_XLSXIO
#include <xlsxio_read.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../utils/memory.h"

#define SKIP_RANGE(val, minVal, maxVal) if(val >= minVal && val < maxVal) continue
#define DAYS_OF_THE_WEEK 5
#define SCHEDULE_BLOCKS 8
#define SUBJECT_NAME_MAX_LENGTH 256

struct schedule_data_t {
    char first_period[DAYS_OF_THE_WEEK][SCHEDULE_BLOCKS][SUBJECT_NAME_MAX_LENGTH];
    char second_period[DAYS_OF_THE_WEEK][SCHEDULE_BLOCKS][SUBJECT_NAME_MAX_LENGTH];
};
struct com_id_t {
    char name[6];
};


int list_page_callback(const XLSXIOCHAR* name, void* callbackdata);
void set_subjet_name_for_block(schedule_data_t* first_period, size_t day, size_t block, char* name);
char* get_subjet_name_for_block(schedule_data_t* first_period, size_t day, size_t block);


schedule_file_t open_file(const char* path) {
    return xlsxioread_open(path);
}

void close_file(schedule_file_t file) {
    xlsxioread_close(file);
}


void get_comissions(schedule_file_t file, size_t* o_comissionCount, com_id_t** o_comissionIds) {
    dynarr_t* data = init_dynamic_array(sizeof(com_id_t), 8);
    xlsxioread_list_sheets(file, list_page_callback, (void*) data); 
    size_t comCount = dynamic_array_size(data);
    size_t newArrSizeBytes = comCount * sizeof(com_id_t);
    (*o_comissionCount) = comCount;
    (*o_comissionIds) = malloc(newArrSizeBytes);
    dynamic_array_copy_to((void*)*o_comissionIds, newArrSizeBytes, data);
    free_dynamic_array(data);
}

void get_schedule_for_comission(schedule_file_t file, com_id_t* comId, schedule_data_t** o_schedule) {
    xlsxioreadersheet sheet = xlsxioread_sheet_open(file, comId->name, XLSXIOREAD_SKIP_EMPTY_ROWS);
    size_t lastColumn = xlsxioread_sheet_last_column_index(sheet);
    size_t lastRow = xlsxioread_sheet_last_row_index(sheet);
    size_t rowIndex = 0;
    size_t periodRowIndex = 0;
    size_t cellIndex = 0;
    (*o_schedule) = create_schedule();
    for (rowIndex = 0; xlsxioread_sheet_next_row(sheet); rowIndex++) {
        const size_t blockIndex = periodRowIndex / 3;
        SKIP_RANGE(rowIndex, 0, 5); // Skip affiliation
        SKIP_RANGE(blockIndex, 8, 10000);
        char* cellValue;
        size_t cellIndex = 0;
        for (cellIndex = 0; (cellValue = xlsxioread_sheet_next_cell(sheet)) != NULL; cellIndex++) {
            const size_t dayIndex = cellIndex - 2;
            SKIP_RANGE(cellIndex, 0, 2);
            char* prevName = get_subjet_name_for_block(*o_schedule, dayIndex, blockIndex);
            const size_t cellSize = strlen(cellValue) * sizeof(char);
            const size_t prevNameSize = strlen(prevName) * sizeof(char);
            const size_t newNameSize = cellSize + prevNameSize + 1;
            char* newName = calloc(cellSize + prevNameSize + 1, sizeof(char));
            strcat_s(newName, newNameSize, prevName);
            strcat_s(newName, newNameSize, cellValue);
            xlsxioread_free(cellValue);
            set_subjet_name_for_block(*o_schedule, dayIndex, blockIndex, newName);
            free(newName);
        }
        periodRowIndex++;
    }
    xlsxioread_sheet_close(sheet);

    for (size_t b = 0; b < SCHEDULE_BLOCKS; b++) {
        printf("\033[31m|\033[0m");
        for (size_t d = 0; d < DAYS_OF_THE_WEEK; d++) {
            printf("%s\033[31m|\033[0m", (*o_schedule)->first_period[d][b]);
        }
        printf("\n");
    }
}


schedule_data_t* create_schedule() {
    schedule_data_t* sch = calloc(sizeof(schedule_data_t), sizeof(char));
    return sch;
}

void delete_schedule(const schedule_data_t* schedule) {
    free(schedule);
}












int list_page_callback(const XLSXIOCHAR* name, void* callbackdata) {
    dynarr_t* data = (dynarr_t*)callbackdata;
    com_id_t* elem = (com_id_t*) push_slot(data);
    memset(elem->name, '\0', 6 * sizeof(char));
    memcpy_s(elem->name, 5, name, strlen(name));
    return 0;
}

void set_subjet_name_for_block(schedule_data_t* first_period, size_t day, size_t block, char* name) {
    char* sub = first_period->first_period[day][block];
    memset(sub, '\0', SUBJECT_NAME_MAX_LENGTH);
    memcpy_s(sub, SUBJECT_NAME_MAX_LENGTH, name, strlen(name) + 1);
}

char* get_subjet_name_for_block(schedule_data_t* first_period, size_t day, size_t block) {
    return first_period->first_period[day][block];
}