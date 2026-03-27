#ifndef STRING_BUILDER_H_
#define STRING_BUILDER_H_

#include "allocator.h"

#include <stdint.h>
#include "string_slice.h"
#include "dynamic_array.h"

typedef struct {
    char * data;
    uint64_t size;
    uint64_t capacity;
} StringBuilder;

void StringBuilderNew(StringBuilder * self);
void StringBuilderPushChar(StringBuilder * self, const char c, allocator_t * a);
void StringBuilderPushString(StringBuilder * self, const char * s, allocator_t * a);
void StringBuilderReset(StringBuilder * self);
void StringBuilderDelete(StringBuilder * self, allocator_t * a);
char * StringBuilderBuild(StringBuilder * self, allocator_t * a);
StringSlice StringBuilderReturnSlice(StringBuilder * self);

#endif
