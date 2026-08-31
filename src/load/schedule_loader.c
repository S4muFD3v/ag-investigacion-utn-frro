#include "schedule_loader.h"
#include <xlsxio_read.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



schedule_file_t open_file(const char* path) {
    return xlsxioread_open(path);
}

void close_file(schedule_file_t file) {
    xlsxioread_close(file);
}

int list_page_callback(const XLSXIOCHAR* name, void* callbackdata);

void get_comissions(schedule_file_t file, size_t* o_comissionCount, com_id_t** o_comissionIds) {
    xlsxioread_list_sheets(file, list_page_callback, NULL);
}

int list_page_callback(const XLSXIOCHAR* name, void* callbackdata) {
    const size_t length = strlen(name);
    char number[3];
    printf("%s:callbackdata=&%p", callbackdata);
    memcpy(number, name + (length - 2), 3);
    size_t id = atoi(number);
}
