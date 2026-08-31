#include "memory.h"
#include <stdlib.h>
#include <string.h>

struct dynarr_t {
    void* ptr;
    size_t capacity;
    size_t size;
    size_t elementSize;
};

dynarr_t* init_dynamic_array(size_t elementSize, size_t reserveCapacity) {
    size_t capacityBytes = elementSize * reserveCapacity;
    dynarr_t* dynarrObject = malloc(sizeof(dynarr_t));
    dynarrObject->ptr = malloc(capacityBytes);
    dynarrObject->capacity = reserveCapacity;
    dynarrObject->size = 0;
    dynarrObject->elementSize = elementSize;
    return dynarrObject;
}

void free_dynamic_array(dynarr_t* array) {
    free(array->ptr);
    free(array);
}

eptr_t push_slot(dynarr_t* array) {
    size_t newSize = array->size + 1;
    eptr_t eptr = NULL;
    if(newSize <= array->capacity) { //aca no tendria que ser <= ? vos habias puesto <, por eso pregunto
        array->size = newSize;
    } else {
        //Realloc
        size_t newCapacity = array->capacity * 1.5;
        size_t newCapacityBytes = newCapacity * array->elementSize;
        size_t copySize = array->elementSize * array->size;
        void* newPtr = malloc(newCapacityBytes);
        if (newPtr == NULL) return NULL;
        memcpy(newPtr, array->ptr, copySize);
        free(array->ptr);
        array->ptr = newPtr;
        array->capacity = newCapacity;
        array->size = newSize;
    }
    return at(array, newSize-1);
}

eptr_t at(dynarr_t* array, size_t position) {
    return (eptr_t)((char*)array->ptr + (position * array->elementSize));
}

size_t dynamic_array_size(dynarr_t* array) {
    return array->size;
}

size_t dynamic_array_capacity(dynarr_t* array) {
    return array->capacity;
}

void dynamic_array_copy_to(void* dest, size_t destSizeBytes, dynarr_t* array) {
    memcpy_s(dest, destSizeBytes, array->ptr, array->size * array->elementSize);
}
