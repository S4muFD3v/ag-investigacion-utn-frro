#ifndef _SCHEDULE_LOADER_H_
#define _SCHEDULE_LOADER_H_

#include <stddef.h>

typedef enum {
    PERIOD_FIRST,
    PERIOD_SECOND,
    PERIOD_MAX_ENUM
} period_t;

typedef struct schedule_data_t schedule_data_t;
typedef void* schedule_file_t;
typedef struct com_id_t com_id_t;


schedule_file_t open_file(const char* path);
void close_file(schedule_file_t file);
void get_comissions(schedule_file_t file, size_t* o_comissionCount, com_id_t** o_comissionIds);
schedule_data_t* get_schedule_for_comission(schedule_file_t file, com_id_t* comId);

schedule_data_t* create_schedule(); // No deberia llamarse, usar get_schedule_for_comission sobre un doble puntero
void delete_schedule(const schedule_data_t* schedule);
void set_subjet_name_for_block(schedule_data_t* schedule, size_t day, size_t block, size_t period, char* name);
char* get_subjet_name_for_block(const schedule_data_t* schedule, size_t day, size_t block, size_t period);
void print_schedule(const schedule_data_t* schedule);

size_t get_schedule_day_count();
size_t get_schedule_block_count();


size_t sizeof_com_id();

#endif
