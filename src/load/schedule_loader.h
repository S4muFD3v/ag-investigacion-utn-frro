#ifndef _SCHEDULE_LOADER_H_
#define _SCHEDULE_LOADER_H_

#include <stddef.h>

typedef struct schedule_data_t schedule_data_t;
typedef void* schedule_file_t;
typedef struct com_id_t com_id_t;


schedule_file_t open_file(const char* path);
void close_file(schedule_file_t file);
void get_comissions(schedule_file_t file, size_t* o_comissionCount, com_id_t** o_comissionIds);
void get_schedule_for_comission(schedule_file_t file, com_id_t* comId, schedule_data_t* o_schedule);

schedule_data_t* create_schedule();
void delete_schedule(const schedule_data_t* schedule);


#endif
