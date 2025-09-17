#include "string_builder.h"
#include <stdlib.h>
#include "util.h"
#include <string.h>

void StringBuilderNew(StringBuilder *self) {
    self->data = 0;
    self->size = 0;
    self->capacity = 0;
}

void _StringBuilderExpand(StringBuilder *self, Allocator * a) {
    if(self->capacity == 0) {
        char * data = a->Alloc(sizeof(char));
        if(data == NULL) terminate("Failed to initially allocate memory for string builder\n");

        self->data = data;
        self->capacity = 1;
        self->size = 0;

        return;
    }

    uint64_t new_capacity = self->capacity * 2;
    char * old_data = self->data;
    self->data = a->Alloc(new_capacity * sizeof(char));
    if(self->data == NULL) terminate("Failed to allocate memory for string builder\n");

    memcpy(self->data, old_data, self->size * sizeof(char));
    a->Free(old_data);
    self->capacity = new_capacity;

}

int _StringBuilderNeedExpanding(StringBuilder * self) {
    return self->size == self->capacity;
}

void StringBuilderPushChar(StringBuilder *self, const char c, Allocator * a) {
    if(_StringBuilderNeedExpanding(self)) _StringBuilderExpand(self, a);
    self->data[self->size++] = c;
}

void StringBuilderPushString(StringBuilder *self, const char *s, Allocator * a) {
    while(*s) { StringBuilderPushChar(self, *s, a); s++; }
}

void StringBuilderReset(StringBuilder *self) {
    self->size = 0;
}

void StringBuilderDelete(StringBuilder *self, Allocator * a) {
    a->Free(self->data);
    self->capacity = 0;
    self->size = 0;
}

char *StringBuilderBuild(StringBuilder *self, Allocator * a) {
    char * out = a->AllocClean(self->size + 1, sizeof(char));
    if(out == NULL) terminate("Failed to allocate out buffer for string builder\n");
    memcpy(out, self->data, self->size * sizeof(char));
    return out;
}

StringSlice StringBuilderReturnSlice(StringBuilder *self) {
    StringSlice s;
    StringSliceNew(&s, self->data, self->size);
    return s;
}
