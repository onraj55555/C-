#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <stddef.h>

typedef struct allocator_t allocator_t;

struct allocator_t {
    void * (*m_alloc)(size_t size);
    void * (*m_clean_alloc)(size_t n, size_t size);
    void * (*m_realloc)(void * p, size_t size);
    void (*m_free)(void * p);
};

static inline void * allocator_alloc(allocator_t * self, size_t size) {
    return self->m_alloc(size);
}

static inline void * allocator_clean_alloc(allocator_t * self, size_t n, size_t size) {
    return self->m_clean_alloc(n, size);
}

static inline void * allocator_realloc(allocator_t * self, void * p, size_t size) {
    return self->m_realloc(p, size);
}

static inline void allocator_free(allocator_t * self, void * p) {
    self->m_free(p);
}
#endif


#ifdef ALLOCATOR_HEAP_ALLOCATOR
#ifndef _ALLOCATOR_HEAP_ALLOCATOR
#define _ALLOCATOR_HEAP_ALLOCATOR
#include <stdlib.h>
static inline void allocator_new_heap_allocator(allocator_t * self) {
    self->m_alloc = &malloc;
    self->m_clean_alloc = &calloc;
    self->m_realloc = &realloc;
    self->m_free = &free;
}
#endif
#endif
