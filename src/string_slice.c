#include "string_slice.h"

void StringSliceNew(StringSlice *self, char *data, uint64_t len) {
    self->data = data;
    self->len = len;
}

char StringSliceAt(StringSlice *self, uint64_t index) {
    if(index < self->len) return self->data[index];
    return (char)0;
}
