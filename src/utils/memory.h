#ifndef _MEMORY_H_
#define _MEMORY_H_


typedef struct dynarr_t dynarr_t;
typedef void* eptr_t;

dynarr_t* init_dynamic_array(size_t elementSize, size_t reserveCapacity);
void free_dynamic_array(dynarr_t* array);


eptr_t push_slot(dynarr_t* array);
eptr_t at(dynarr_t* array, size_t position);



#endif