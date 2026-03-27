#include "dynamic_array.h"
#include <stdint.h>
#define ALLOCATOR_HEAP_ALLOCATOR
#include "allocator.h"
#include <assert.h>
#include <stdio.h>

typedef DA_CREATE_STRUCT(da_int, int) da_int;

int main() {
    da_int daint;
    DA_INIT(&daint);

    allocator_t a;
    allocator_new_heap_allocator(&a);

    for(int i = 0; i < 100; i++) {
        printf("Pushing %d\n", i);
        DA_PUSHBACK(&daint, i, &a);
    }
    puts("Done pushing");

    for(int i = 0; i < 100; i++) {
        assert(i == daint.data[i]);
    }
    assert(daint.capacity == 2 * DA_INIT_CAPACITY);
    DA_DEINIT(&daint, &a);

    return 0;
}