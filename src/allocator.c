#include "allocator.h"
#include <stdlib.h>

void AllocatorHeapAllocatorNew(Allocator *self) {
    self->Alloc = &malloc;
    self->AllocClean = &calloc;
    self->Free = &free;
}