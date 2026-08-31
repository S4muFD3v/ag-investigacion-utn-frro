#include "schedule_loader.h"
#define BUILD_XLSXIO_STATIC
#define BUILD_XLSXIO
#include <xlsxio_read.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../utils/memory.h"

#define SKIP_INTERVAL(val, minVal, maxVal) if(val >= minVal && val < maxVal) continue
#define DAYS_OF_THE_WEEK 7
#define SCHEDULE_BLOCKS 8
#define SUBJECT_NAME_MAX_LENGTH 156

struct schedule_data_t {
    char schedule[DAYS_OF_THE_WEEK][SCHEDULE_BLOCKS][SUBJECT_NAME_MAX_LENGTH];
};
struct com_id_t {
    char name[6];
};


int list_page_callback(const XLSXIOCHAR* name, void* callbackdata);
void set_subjet_name_for_block(schedule_data_t* schedule, size_t day, size_t block, char* name);
char* get_subjet_name_for_block(schedule_data_t* schedule, size_t day, size_t block);


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
    schedule_data_t data;
    size_t blockIndex = 0;
    size_t dayIndex = 0;
    while(xlsxioread_sheet_next_row(sheet)) { 
        rowIndex++;
        SKIP_INTERVAL(rowIndex, 1, 6); // Skip affiliation
        char* cellValue;
        size_t cellIndex = 0;
        while((cellValue = xlsxioread_sheet_next_cell(sheet)) != NULL) {
            cellIndex++;
            SKIP_INTERVAL(cellIndex, 1, 3);
            //set_subjet_name_for_block(&data, dayIndex, blockIndex, cellValue);
        }
    }
    xlsxioread_sheet_close(sheet);
}















int list_page_callback(const XLSXIOCHAR* name, void* callbackdata) {
    dynarr_t* data = (dynarr_t*)callbackdata;
    com_id_t* elem = (com_id_t*) push_slot(data);
    memset(elem->name, '\0', 6 * sizeof(char));
    memcpy_s(elem->name, 5, name, strlen(name));
    return 0;
}

void set_subjet_name_for_block(schedule_data_t* schedule, size_t day, size_t block, char* name) {
    char* sub = schedule->schedule[day][block];
    memset(sub, '\0', SUBJECT_NAME_MAX_LENGTH);
    memcpy_s(sub, SUBJECT_NAME_MAX_LENGTH, name, strlen(name));
}

char* get_subjet_name_for_block(schedule_data_t* schedule, size_t day, size_t block) {
    return schedule->schedule[day][block];
}