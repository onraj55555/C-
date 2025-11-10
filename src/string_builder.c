#include "string_builder.h"
#include "allocator.h"
#include "util.h"
#include <string.h>

void StringBuilderNew(StringBuilder *self) {
    self->data = 0;
    self->size = 0;
    self->capacity = 0;
}

void _StringBuilderExpand(StringBuilder *self, allocator_t * a) {
    if(self->capacity == 0) {
        char * data = allocator_alloc(a, sizeof(char));
        if(data == NULL) terminate("Failed to initially allocate memory for string builder\n");

        self->data = data;
        self->capacity = 1;
        self->size = 0;

        return;
    }

    uint64_t new_capacity = self->capacity * 2;
    char * old_data = self->data;
    self->data = allocator_alloc(a, new_capacity * sizeof(char));
    if(self->data == NULL) terminate("Failed to allocate memory for string builder\n");

    memcpy(self->data, old_data, self->size * sizeof(char));
    allocator_free(a, old_data);
    self->capacity = new_capacity;

}

int _StringBuilderNeedExpanding(StringBuilder * self) {
    return self->size == self->capacity;
}

void StringBuilderPushChar(StringBuilder *self, const char c, allocator_t * a) {
    if(_StringBuilderNeedExpanding(self)) _StringBuilderExpand(self, a);
    self->data[self->size++] = c;
}

void StringBuilderPushString(StringBuilder *self, const char *s, allocator_t * a) {
    while(*s) { StringBuilderPushChar(self, *s, a); s++; }
}

void StringBuilderReset(StringBuilder *self) {
    self->size = 0;
}

void StringBuilderDelete(StringBuilder *self, allocator_t * a) {
    allocator_free(a, self->data);
    self->capacity = 0;
    self->size = 0;
}

char *StringBuilderBuild(StringBuilder *self, allocator_t * a) {
    char * out = allocator_clean_alloc(a, self->size + 1, sizeof(char));
    if(out == NULL) terminate("Failed to allocate out buffer for string builder\n");
    memcpy(out, self->data, self->size * sizeof(char));
    return out;
}

StringSlice StringBuilderReturnSlice(StringBuilder *self) {
    StringSlice s;
    StringSliceNew(&s, self->data, self->size);
    return s;
}
