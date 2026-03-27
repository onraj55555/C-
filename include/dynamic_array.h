#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include <stdint.h>
#include <stdlib.h>
#include "allocator.h"
#include "util.h"
#include "string.h"

/*
    Dynamic arrays should be defined as follows
    typedef struct dynamic_array_name {
        uint64_t size;
        uint64_t capacity;
        custom_type * data;
    } dynamic_array_name;

    The macros can be used as follows
    DA_INIT(&dynamic_array_object);
    Note: arrays should be provided BY POINTER (makes things cleaner because usually you will use the array pointer instead of the array)
*/

#define DA_CREATE_STRUCT(name, type) struct name { uint64_t size; uint64_t capacity; type * data; }

#define DA_INIT_CAPACITY 64

typedef struct _da_template {
    uint64_t size;
    uint64_t capacity;
    void * data;
} _da_template;

void _da_init(_da_template * da);
void _da_deinit(_da_template * da, allocator_t * a);
void _da_expand(_da_template * da, size_t element_size, allocator_t * a);
void _da_pushback(_da_template * da, size_t element_size, void * element, allocator_t * a);

#define DA_INIT(dynamic_array) _da_init((_da_template *)(dynamic_array))
#define DA_PUSHBACK(dynamic_array, element, allocator) _da_pushback((_da_template *)(dynamic_array), sizeof((dynamic_array)->data[0]), (void *)(&element), allocator)
#define DA_DEINIT(dynamic_array, allocator) _da_deinit((_da_template *)(dynamic_array), allocator)

#endif