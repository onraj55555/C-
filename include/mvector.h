/*
---------- USAGE ----------
1. define MVECTOR_TYPE
2. include mvector.h to get struct type, extern error storage definition and function definitions
3. define MVECTOR_IMPL
3. include mvector.h to get error storage and function implementations

Optionally undef both macros to prevent compiler errors when using the vector macros for multiple types
*/

#ifndef MVECTOR_H_UTILS
#define MVECTOR_H_UTILS

#define MVECTOR_CONCAT2(a, b) a##b
#define MVECTOR_CONCAT(a, b) MVECTOR_CONCAT2(a, b)

typedef enum {
    ok = 0,
    alloc,
    empty,
    out_of_bounds,
    invalid_value
} vector_error_t;

#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef MVECTOR_TYPE
#define MVECTOR_PREFIX MVECTOR_CONCAT(vector_, MVECTOR_TYPE)

extern int MVECTOR_CONCAT(MVECTOR_PREFIX, _error);

typedef struct {
    MVECTOR_TYPE * data;
    int64_t size;
    int64_t capacity;
} MVECTOR_PREFIX;

// void vector_int_new(vector_int * self);
void  MVECTOR_CONCAT(MVECTOR_PREFIX, _new)(MVECTOR_PREFIX * self);

// void vector_int_new_reserve(vector_int * self, int64_t capacity)
void MVECTOR_CONCAT(MVECTOR_PREFIX, _new_reserve)(MVECTOR_PREFIX * self, int64_t capacity);

// int64_t vector_int_pushback(vector_int * self, int * value);
int64_t MVECTOR_CONCAT(MVECTOR_PREFIX, _pushback)(MVECTOR_PREFIX * self, MVECTOR_TYPE * value);

// void vector_int_popback(vector_int * self, int * dest);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _popback)(MVECTOR_PREFIX * self, MVECTOR_TYPE * dest);

// void vector_int_get(vector_int * self, int64_t index, int * dest);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _get)(MVECTOR_PREFIX * self, int64_t index, MVECTOR_TYPE * dest);

// int * vector_int_get_ref(vector_int * self, int64_t index);
MVECTOR_TYPE * MVECTOR_CONCAT(MVECTOR_PREFIX, _get_ref)(MVECTOR_PREFIX * self, int64_t index);

// void vector_int_insert_at(vector_int * self, int64_t index, int * value);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _insert_at)(MVECTOR_PREFIX * self, int64_t index, MVECTOR_TYPE * value);

// void vector_int_delete(vector_int * self)
void MVECTOR_CONCAT(MVECTOR_PREFIX, _delete)(MVECTOR_PREFIX * self);

#ifdef MVECTOR_IMPL

int MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = 0;

// void vector_int_new(vector_int * self);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _new)(MVECTOR_PREFIX * self) {
    self->data = 0;
    self->size = 0;
    self->capacity = 0;
}

// void vector_int_new_reserve(vector_int * self, int64_t capacity)
void MVECTOR_CONCAT(MVECTOR_PREFIX, _new_reserve)(MVECTOR_PREFIX * self, int64_t capacity) {
    if(capacity < 0) {
        MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = invalid_value;
        return;
    }

    self->data = (MVECTOR_TYPE *) malloc(capacity * sizeof(MVECTOR_TYPE));
    
    if(self->data == 0) {
        MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = alloc;
        return;
    }

    self->size = 0;
    self->capacity = capacity;
}

// int64_t vector_int_pushback(vector_int * self, int * value);
int64_t MVECTOR_CONCAT(MVECTOR_PREFIX, _pushback)(MVECTOR_PREFIX * self, MVECTOR_TYPE * value) {
    if(self->capacity == 0) {
        self->data = (MVECTOR_TYPE *)malloc(1 * sizeof(MVECTOR_TYPE));

        if(self->data == 0) {
            MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = alloc;
            return -1;
        }
        self->capacity = 1;
    } else if(self->size == self->capacity) {
        MVECTOR_TYPE * temp = (MVECTOR_TYPE *)realloc(self->data, 2 * self->capacity * sizeof(MVECTOR_TYPE));

        if(temp == 0) {
            MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = alloc;
            return -1;
        }

        self->data = temp;
        self->capacity = 2 * self->capacity;
    }

    memcpy(self->data + self->size, value, sizeof(MVECTOR_TYPE));

    return self->size++;
}

// void vector_int_popback(vector_int * self, int * dest);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _popback)(MVECTOR_PREFIX * self, MVECTOR_TYPE * dest) {
    if(self->size == 0) {
        MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = empty;
        return;
    }

    if(dest) {
        memcpy(dest, self->data + self->size - 1, sizeof(MVECTOR_TYPE));
    }

    self->size--;
}

// void vector_int_get(vector_int * self, int64_t index, int * dest);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _get)(MVECTOR_PREFIX * self, int64_t index, MVECTOR_TYPE * dest) {
    // Wrap around
    if(index < 0) {
        index += self->size;
    }

    if(index < 0 || index >= self->size) {
        MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = out_of_bounds;
        return;
    }

    memcpy(dest, self->data + index, sizeof(MVECTOR_TYPE));
}

// int * vector_int_get_ref(vector_int * self, int64_t index);
MVECTOR_TYPE * MVECTOR_CONCAT(MVECTOR_PREFIX, _get_ref)(MVECTOR_PREFIX * self, int64_t index) {
    // Wrap around
    if(index < 0) {
        index += self->size;
    }

    if(index < 0 || index >= self->size) {
        MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = out_of_bounds;
        return 0;
    }

    return self->data + index;
}

// void vector_int_insert_at(vector_int * self, int64_t index, int * value);
void MVECTOR_CONCAT(MVECTOR_PREFIX, _insert_at)(MVECTOR_PREFIX * self, int64_t index, MVECTOR_TYPE * value) {
    // Wrap around
    if(index < 0) {
        index += self->size;
    }

    if(index < 0 || index >= self->size) {
        MVECTOR_CONCAT(MVECTOR_PREFIX, _error) = out_of_bounds;
        return;
    }

    memcpy(self->data + index, value, sizeof(MVECTOR_TYPE));
}

// void vector_int_delete(vector_int * self)
void MVECTOR_CONCAT(MVECTOR_PREFIX, _delete)(MVECTOR_PREFIX * self) {
    free(self->data);
    self->data = 0;
    self->size = 0;
    self->capacity = 0;
}
#endif

#undef MVECTOR_TYPE
#undef MVECTOR_PREFIX

#endif