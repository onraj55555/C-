#include "compilation_unit.h"
#include "lexer.h"
#include "allocator.h"
#include <stdlib.h>

int main() {
    char * path = "test-examples/1.cb";
    CompilationUnit * cu = malloc(sizeof(CompilationUnit));
    CompilationUnitFromFile(cu, path);

    Allocator * a = malloc(sizeof(Allocator));
    AllocatorHeapAllocatorNew(a);

    Lexer * l = malloc(sizeof(Lexer));
    LexerNew(l);
    LexerTokenise(l, cu, a);
    LexerPrintInitialisation(l);

    return 0;
}
