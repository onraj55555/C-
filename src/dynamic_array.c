#include "dynamic_array.h"

void _da_init(_da_template * da) {
    da->size = 0;
    da->capacity = 0;
    da->data = 0;
}

void _da_deinit(_da_template * da, allocator_t * a) {
    da->size = 0;
    da->capacity = 0;
    allocator_free(a, da->data);
}

void _da_expand(_da_template * da, size_t element_size, allocator_t * a) {
    size_t new_capacity = 0;
    if(0 == da->capacity) {
        new_capacity = DA_INIT_CAPACITY;
    } else {
        new_capacity = da->capacity * 2;
    }
    void * new_data = allocator_alloc(a, new_capacity * element_size);
    if(!new_data) terminate("Could not allocate data for dynamic array");
    memcpy(new_data, da->data, da->capacity * element_size);
    allocator_free(a, da->data);
    da->data = new_data;
    da->capacity = new_capacity;
}

void _da_pushback(_da_template * da, size_t element_size, void * element, allocator_t * a) {
    if(da->size == da->capacity) {
        _da_expand(da, element_size, a);
    }
    void * base = ((char *)da->data) + da->size * element_size;
    memcpy(base, element, element_size);
    da->size += 1;
}