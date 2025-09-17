#include "compilation_unit.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_builder.h"

StringSlice _CompilationUnitFileGetLine(CompilationUnit * self, Allocator * a) {
    StringBuilderReset(&self->sb);

    while(1) {
        int c = fgetc(self->data.file_data.file);

        if(c == EOF) {
            self->eof = 1;
            break;
        }

        if(((char)c) == '\n') break; // End of line

        StringBuilderPushChar(&self->sb, (char)c, a);
    }

    return StringBuilderReturnSlice(&self->sb);
}

StringSlice _CompilationUnitStringGetLine(CompilationUnit * self, Allocator * a) {
    StringBuilderReset(&self->sb);

    while(1) {
        char c = *self->data.string_data.slider;
        self->data.string_data.slider++;

        if(c == 0) {
            self->eof = 1;
            break;
        }

        if(c == '\n') break; // End of line

        StringBuilderPushChar(&self->sb, c, a);
    }

    return StringBuilderReturnSlice(&self->sb);
}

void _CompilationUnitNewGeneral(CompilationUnit * self, char * path) {
    StringBuilderNew(&self->sb);
    self->path = path;
    self->eof = 0;
}

void CompilationUnitFromString(CompilationUnit *self, char * name, char *s) {
    self->type = COMPILATION_UNIT_STRING;
    _CompilationUnitNewGeneral(self, name);

    self->data.string_data.string = s;
    self->data.string_data.slider = s;
}

void CompilationUnitFromFile(CompilationUnit *self, char *path) {
    self->type = COMPILATION_UNIT_FILE;
    _CompilationUnitNewGeneral(self, path);

    FILE * f = fopen(path, "r");
    if(f == NULL) {
        terminate("Failed to open file '%s'\n", path);
    }

    self->path = strdup(path);
    if(self->path == NULL) {
        terminate("Failed to duplicate path '%s'\n", path);
    }

    self->data.file_data.file = f;
}

StringSlice CompilationUnitGetLine(CompilationUnit *self, Allocator * a) {
    switch(self->type) {
        case COMPILATION_UNIT_FILE: return _CompilationUnitFileGetLine(self, a); break;
        case COMPILATION_UNIT_STRING: return _CompilationUnitStringGetLine(self, a); break;
    }
}

int CompilationUnitHasLine(CompilationUnit *self) {
    return self->eof == 0;
}

void CompilationUnitDelete(CompilationUnit *self, Allocator *a) {
    StringBuilderDelete(&self->sb, a);
    if(self->type == COMPILATION_UNIT_FILE) fclose(self->data.file_data.file);
}
