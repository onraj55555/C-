#include "util.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void terminate(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(EXIT_FAILURE);
}