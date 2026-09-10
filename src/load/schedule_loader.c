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

typedef char chart_t[DAYS_OF_THE_WEEK][SCHEDULE_BLOCKS][SUBJECT_NAME_MAX_LENGTH];

typedef enum {
    READER_STATUS_WAITING_FOR_HEADER,
    READER_STATUS_READING_CHART,
    READER_STATUS_FINISHED,
} reader_status_t;

struct schedule_data_t {
    chart_t first_period;
    chart_t second_period;
};

struct com_id_t {
    char name[6];
};




schedule_file_t open_file(const char* path) {
    return xlsxioread_open(path);
}

void close_file(schedule_file_t file) {
    xlsxioread_close(file);
}

int list_page_callback(const XLSXIOCHAR* name, void* callbackdata);

void get_comissions(schedule_file_t file, size_t* o_comissionCount, com_id_t** o_comissionIds) {
    dynarr_t* data = init_dynamic_array(sizeof(com_id_t), 8);
    xlsxioread_list_sheets(file, list_page_callback, (void*) data); 
    size_t comCount = dynamic_array_size(data);
    size_t newArrSizeBytes = comCount * sizeof(com_id_t);
    (*o_comissionCount) = comCount;
    (*o_comissionIds) = malloc(newArrSizeBytes);
    dynamic_array_copy_to(*o_comissionIds, newArrSizeBytes, data);
    free_dynamic_array(data);
}

int list_page_callback(const XLSXIOCHAR* name, void* callbackdata) {
    dynarr_t* data = (dynarr_t*)callbackdata;
    com_id_t* elem = (com_id_t*)push_slot(data);
    memset(elem->name, '\0', 6 * sizeof(char));
    memcpy_s(elem->name, 5, name, strlen(name));
    return 0;
}


void read_schedule_chart(xlsxioreadersheet sheet, schedule_data_t* o_schedule, size_t periodRowIndex, period_t period);
int find_header(xlsxioreadersheet sheet, const char** headers, size_t headerCount);

schedule_data_t* get_schedule_for_comission(schedule_file_t file, com_id_t* comId) {
    schedule_data_t* o_schedule = create_schedule();
    xlsxioreadersheet sheet = xlsxioread_sheet_open(file, comId->name, XLSXIOREAD_SKIP_EMPTY_ROWS);
    size_t periodRowIndex = 0;
    period_t period = PERIOD_MAX_ENUM;
    reader_status_t status = READER_STATUS_WAITING_FOR_HEADER;
    const char* headers[2] = {
        "Primer Cuatrimestre",
        "Segundo Cuatrimestre"
    };
    while (xlsxioread_sheet_next_row(sheet)) {
        switch (status) {
        case READER_STATUS_WAITING_FOR_HEADER:
            int header = -1;
            if ((header = find_header(sheet, headers, 2)) >= 0) {
                status = READER_STATUS_READING_CHART;
                period = (period_t) header;
                periodRowIndex = 0;
                xlsxioread_sheet_next_row(sheet); // Skip one rows
            }  
            break;
        case READER_STATUS_READING_CHART:
            read_schedule_chart(sheet, o_schedule, periodRowIndex, period);
            periodRowIndex++;
            if(periodRowIndex >= (SCHEDULE_BLOCKS * 3)) 
                status = period != PERIOD_SECOND ? READER_STATUS_WAITING_FOR_HEADER : READER_STATUS_FINISHED;
            break;
        case READER_STATUS_FINISHED:
        default:
            break;
        }
    }
    xlsxioread_sheet_close(sheet);
    return o_schedule;
}

void read_schedule_chart(
    xlsxioreadersheet sheet, schedule_data_t* o_schedule, size_t periodRowIndex, period_t period) {
    const size_t blockIndex = periodRowIndex / 3;
    char* cellValue;
    size_t cellIndex = 0;
    for (cellIndex = 0; (cellValue = xlsxioread_sheet_next_cell(sheet)) != NULL; cellIndex++) {
        const size_t dayIndex = cellIndex - 2;
        SKIP_RANGE(cellIndex, 0, 2);
        SKIP_RANGE(dayIndex, 5, 1000000);
        char* prevName = get_subjet_name_for_block(o_schedule, dayIndex, blockIndex, period);
        const size_t cellSize = strlen(cellValue) * sizeof(char);
        const size_t prevNameSize = strlen(prevName) * sizeof(char);
        const size_t newNameSize = cellSize + prevNameSize + 1;
        char* newName = calloc(cellSize + prevNameSize + 1, sizeof(char));
        strcat_s(newName, newNameSize, prevName);
        strcat_s(newName, newNameSize, " ");
        strcat_s(newName, newNameSize, cellValue);
        xlsxioread_free(cellValue);
        set_subjet_name_for_block(o_schedule, dayIndex, blockIndex, period, newName);
        free(newName);
    }
}

int find_header(xlsxioreadersheet sheet, const char** headers, size_t headerCount) {
    char* cellValue = NULL;
    int foundIn = -1;
    while ((cellValue = xlsxioread_sheet_next_cell(sheet)) != NULL) {
        for (size_t i = 0; i < headerCount;  i++) {
            if (strcmp(cellValue, headers[i]) == 0) {
                foundIn = i;
                break;
            }
        }
        xlsxioread_free(cellValue);
        if (foundIn >= 0) break;
    }
    return foundIn;
}



schedule_data_t* create_schedule() {
    schedule_data_t* sch = calloc(sizeof(schedule_data_t), sizeof(char));
    return sch;
}

void delete_schedule(const schedule_data_t* schedule) {
    free(schedule);
}










void set_subjet_name_for_block(schedule_data_t* first_period, size_t day, size_t block, period_t period, char* name) {
    char* sub = NULL;
    if (!(day < DAYS_OF_THE_WEEK && block < SCHEDULE_BLOCKS)) {
        printf("Unexistent block day=%d block=%d", day, block);
        abort();
    }
    if (period == PERIOD_FIRST)
        sub = first_period->first_period[day][block];
    else if (period == PERIOD_SECOND)
        sub = first_period->second_period[day][block];
    else abort();
    memset(sub, '\0', SUBJECT_NAME_MAX_LENGTH);
    memcpy_s(sub, SUBJECT_NAME_MAX_LENGTH, name, strlen(name) + 1);
}

char* get_subjet_name_for_block(const schedule_data_t* first_period, size_t day, size_t block, period_t period) {
    if (!(day < DAYS_OF_THE_WEEK && block < SCHEDULE_BLOCKS)) {
        printf("Unexistent block day=%d block=%d", day, block);
        abort();
    }
    if (period == PERIOD_FIRST)
        return first_period->first_period[day][block];
    else if (period == PERIOD_SECOND)
        return first_period->second_period[day][block];
    else abort();
}

void print_schedule(const schedule_data_t* schedule) {
    printf("\n----------Primero-----------\n");
    for (size_t b = 0; b < SCHEDULE_BLOCKS; b++) {
        printf("\033[31m|\033[0m");
        for (size_t d = 0; d < DAYS_OF_THE_WEEK; d++) {
            printf("%s\033[31m|\033[0m", schedule->first_period[d][b]);
        }
        printf("\n");
    }
    printf("\n----------Segundo-----------\n");
    for (size_t b = 0; b < SCHEDULE_BLOCKS; b++) {
        printf("\033[31m|\033[0m");
        for (size_t d = 0; d < DAYS_OF_THE_WEEK; d++) {
            printf("%s\033[31m|\033[0m", schedule->second_period[d][b]);
        }
        printf("\n");
    }
}




size_t sizeof_com_id() {
    return sizeof(com_id_t);
}


size_t get_schedule_day_count() {
    return DAYS_OF_THE_WEEK;
}

size_t get_schedule_block_count() {
    return SCHEDULE_BLOCKS;
}