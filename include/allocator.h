#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <stddef.h>

typedef struct {
    void * (*Alloc)(size_t size);
    void * (*AllocClean)(size_t n, size_t size);
    void (*Free)(void * p);
} Allocator;

void AllocatorHeapAllocatorNew(Allocator * self);

#endif