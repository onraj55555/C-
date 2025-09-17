#ifndef STRING_BUILDER_H_
#define STRING_BUILDER_H_

#include <stdint.h>
#include "allocator.h"
#include "string_slice.h"

typedef struct {
    char * data;
    uint64_t size;
    uint64_t capacity;
} StringBuilder;

void StringBuilderNew(StringBuilder * self);
void StringBuilderPushChar(StringBuilder * self, const char c, Allocator * a);
void StringBuilderPushString(StringBuilder * self, const char * s, Allocator * a);
void StringBuilderReset(StringBuilder * self);
void StringBuilderDelete(StringBuilder * self, Allocator * a);
char * StringBuilderBuild(StringBuilder * self, Allocator * a);
StringSlice StringBuilderReturnSlice(StringBuilder * self);

#endif