#ifndef COMPILATION_UNIT_H_
#define COMPILATION_UNIT_H_

#include <stdio.h>
#include "string_slice.h"
#include "allocator.h"
#include "string_builder.h"

typedef struct CompilationUnit CompilationUnit;
typedef struct _CompilationUnitFileData _CompilationUnitFileData;
typedef struct _CompilationUnitStringData _CompilationUnitStringData;
typedef enum CompilationUnitType CompilationUnitType;

enum CompilationUnitType {
    COMPILATION_UNIT_FILE,
    COMPILATION_UNIT_STRING
};

struct _CompilationUnitFileData {
    FILE * file;
};

struct _CompilationUnitStringData {
    char * string;
    char * slider;
};

struct CompilationUnit {
    CompilationUnitType type;
    StringBuilder sb;
    char * path;
    int eof;

    union {
        _CompilationUnitFileData file_data;
        _CompilationUnitStringData string_data;
    } data;
};

void CompilationUnitFromString(CompilationUnit * self, char * name, char * s);
void CompilationUnitFromFile(CompilationUnit * self, char * path);
StringSlice CompilationUnitGetLine(CompilationUnit * self, Allocator * a);;
int CompilationUnitHasLine(CompilationUnit * self);
void CompilationUnitDelete(CompilationUnit * self, Allocator * a);

#endif