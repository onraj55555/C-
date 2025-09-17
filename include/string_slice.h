#ifndef STRING_SLICE_H_
#define STRING_SLICE_H_

#include <stdint.h>

typedef struct StringSlice StringSlice;

struct StringSlice {
    char * data;
    uint64_t len;
};

void StringSliceNew(StringSlice * self, char * data, uint64_t len);
char StringSliceAt(StringSlice * self, uint64_t index);

#endif