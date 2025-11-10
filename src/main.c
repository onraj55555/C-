#include "compilation_unit.h"
#include "lexer.h"
#define ALLOCATOR_HEAP_ALLOCATOR
#include "allocator.h"
#include <stdlib.h>

int main() {
    char * path = "test-examples/1.cb";
    CompilationUnit * cu = malloc(sizeof(CompilationUnit));
    CompilationUnitFromFile(cu, path);

    allocator_t * a = malloc(sizeof(allocator_t));
    allocator_new_heap_allocator(a);

    Lexer * l = malloc(sizeof(Lexer));
    LexerNew(l);
    LexerTokenise(l, cu, a);
    LexerPrintInitialisation(l);

    return 0;
}
