/*
---------- USAGE ----------
1. define MDA_TYPE
2. include mda.h to get struct type, extern error storage definition and function definitions
3. define MDA_IMPLEMENTATION
3. include mda.h to get error storage and function implementations
*/

#ifndef MDA_H_UTILS
#define MDA_H_UTILS

#define MDA_CONCAT2(a, b) a##b
#define MDA_CONCAT(a, b) MDA_CONCAT2(a, b)

typedef enum {
    ok = 0,
    alloc,
    empty,
    out_of_bounds,
    invalid_value
} da_error_t;

#endif

#ifdef MDA_TYPE

#include <stdint.h>
#include "allocator.h"

#define MDA_PREFIX MDA_CONCAT(da_, MDA_TYPE)

extern int MDA_CONCAT(MDA_PREFIX, _error);

typedef struct {
    MDA_TYPE * data;
    int64_t size;
    int64_t capacity;
} MDA_PREFIX;

// void da_int_new(da_int * self);
void MDA_CONCAT(MDA_PREFIX, _new)(MDA_PREFIX * self);

// void da_int_new_reserve(da_int * self, int64_t capacity)
void MDA_CONCAT(MDA_PREFIX, _new_reserve)(MDA_PREFIX * self, int64_t capacity, allocator_t * a);

// int64_t da_int_pushback(da_int * self, int * value);
int64_t MDA_CONCAT(MDA_PREFIX, _pushback)(MDA_PREFIX * self, MDA_TYPE * value, allocator_t * a);

// void da_int_popback(da_int * self, int * dest);
MDA_TYPE MDA_CONCAT(MDA_PREFIX, _popback)(MDA_PREFIX * self);

// void da_int_get(da_int * self, int64_t index, int * dest);
MDA_TYPE MDA_CONCAT(MDA_PREFIX, _get)(MDA_PREFIX * self, int64_t index);

// int * da_int_get_ref(da_int * self, int64_t index);
MDA_TYPE * MDA_CONCAT(MDA_PREFIX, _get_ref)(MDA_PREFIX * self, int64_t index);

// void da_int_insert_at(da_int * self, int64_t index, int * value);
void MDA_CONCAT(MDA_PREFIX, _insert_at)(MDA_PREFIX * self, int64_t index, MDA_TYPE * value);

// void da_int_delete(da_int * self)
void MDA_CONCAT(MDA_PREFIX, _delete)(MDA_PREFIX * self, allocator_t * a);


// The actual implementation
#ifdef MDA_IMPLEMENTATION

#include <string.h>

int MDA_CONCAT(MDA_PREFIX, _error) = 0;

// void da_int_new(da_int * self);
void MDA_CONCAT(MDA_PREFIX, _new)(MDA_PREFIX * self) {
    self->data = 0;
    self->size = 0;
    self->capacity = 0;
}

// void da_int_new_reserve(da_int * self, int64_t capacity)
void MDA_CONCAT(MDA_PREFIX, _new_reserve)(MDA_PREFIX * self, int64_t capacity, allocator_t * a) {
    if(capacity < 0) {
        MDA_CONCAT(MDA_PREFIX, _error) = invalid_value;
        return;
    }

    self->data = (MDA_TYPE *) allocator_alloc(a, capacity * sizeof(MDA_TYPE));

    if(self->data == 0) {
        MDA_CONCAT(MDA_PREFIX, _error) = alloc;
        return;
    }

    self->size = 0;
    self->capacity = capacity;
}

// int64_t da_int_pushback(da_int * self, int * value);
int64_t MDA_CONCAT(MDA_PREFIX, _pushback)(MDA_PREFIX * self, MDA_TYPE * value, allocator_t * a) {
    if(self->capacity == 0) {
        self->data = (MDA_TYPE *) allocator_alloc(a, 1 * sizeof(MDA_TYPE));

        if(self->data == 0) {
            MDA_CONCAT(MDA_PREFIX, _error) = alloc;
            return -1;
        }
        self->capacity = 1;
    } else if(self->size == self->capacity) {
        MDA_TYPE * temp = (MDA_TYPE *) allocator_realloc(a, self->data, 2 * self->capacity * sizeof(MDA_TYPE));

        if(temp == 0) {
            MDA_CONCAT(MDA_PREFIX, _error) = alloc;
            return -1;
        }

        self->data = temp;
        self->capacity = 2 * self->capacity;
    }

    memcpy(self->data + self->size, value, sizeof(MDA_TYPE));

    return self->size++;
}

// void da_int_popback(da_int * self, int * dest);
MDA_TYPE MDA_CONCAT(MDA_PREFIX, _popback)(MDA_PREFIX * self) {
    if(self->size == 0) {
        MDA_CONCAT(MDA_PREFIX, _error) = empty;
        return (MDA_TYPE) { 0 };
    }

    return self->data[self->size-- - 1];
}

// void da_int_get(da_int * self, int64_t index, int * dest);
MDA_TYPE MDA_CONCAT(MDA_PREFIX, _get)(MDA_PREFIX * self, int64_t index) {
    // Wrap around
    if(index < 0) {
        index += self->size;
    }

    if(index < 0 || index >= self->size) {
        MDA_CONCAT(MDA_PREFIX, _error) = out_of_bounds;
        return (MDA_TYPE) { 0 };
    }

    return self->data[index];
}

// int * da_int_get_ref(da_int * self, int64_t index);
MDA_TYPE * MDA_CONCAT(MDA_PREFIX, _get_ref)(MDA_PREFIX * self, int64_t index) {
    // Wrap around
    if(index < 0) {
        index += self->size;
    }

    if(index < 0 || index >= self->size) {
        MDA_CONCAT(MDA_PREFIX, _error) = out_of_bounds;
        return 0;
    }

    return self->data + index;
}

// void da_int_insert_at(da_int * self, int64_t index, int * value);
void MDA_CONCAT(MDA_PREFIX, _insert_at)(MDA_PREFIX * self, int64_t index, MDA_TYPE * value) {
    // Wrap around
    if(index < 0) {
        index += self->size;
    }

    if(index < 0 || index >= self->size) {
        MDA_CONCAT(MDA_PREFIX, _error) = out_of_bounds;
        return;
    }

    memcpy(self->data + index, value, sizeof(MDA_TYPE));
}

// void da_int_delete(da_int * self)
void MDA_CONCAT(MDA_PREFIX, _delete)(MDA_PREFIX * self, allocator_t * a) {
    allocator_free(a, self->data);
    self->data = 0;
    self->size = 0;
    self->capacity = 0;
}
#endif

#undef MDA_TYPE
#undef MDA_PREFIX

#endif
