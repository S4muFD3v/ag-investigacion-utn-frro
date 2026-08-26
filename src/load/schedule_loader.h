#ifndef _SCHEDULE_LOADER_H_
#define _SCHEDULE_LOADER_H_

typedef void* schedule_file_t;


schedule_file_t open_file(const char* path);
void close_file();



#endif
