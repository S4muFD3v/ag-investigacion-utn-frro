#ifndef _SCHEDULE_LOADER_H_
#define _SCHEDULE_LOADER_H_

#include <stddef.h>

typedef void* schedule_file_t;
typedef size_t com_id_t;


schedule_file_t open_file(const char* path);
void close_file(schedule_file_t file);
void get_comissions(schedule_file_t file, size_t* o_comissionCount, com_id_t** o_comissionIds);


#endif
