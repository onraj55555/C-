#include "compilation_unit.h"
#include "lexer.h"
#include "util.h"
#include <assert.h>
#include <string.h>

void test_lexer() {
    CompilationUnit cu;
    CompilationUnitFromFile(&cu, "test-examples/1.cb");

    Lexer l;
    LexerNew(&l);

    Allocator a;
    AllocatorHeapAllocatorNew(&a);

    Token tokens[] = {
        {.type = Pack, .data = 0, .line = 0, .index = 0, .path = "test-examples/1.cb"}, {.type = Id, .data = "app", .line = 0, .index = 8, .path = "test-examples/1.cb"}, {.type = DotComma, .data = 0, .line = 0, .index = 11, .path = "test-examples/1.cb"}, {.type = Import, .data = 0, .line = 2, .index = 0, .path = "test-examples/1.cb"}, {.type = LBra, .data = 0, .line = 2, .index = 6, .path = "test-examples/1.cb"}, {.type = StringLit, .data = "std", .line = 2, .index = 7, .path = "test-examples/1.cb"}, {.type = RBra, .data = 0, .line = 2, .index = 12, .path = "test-examples/1.cb"}, {.type = DotComma, .data = 0, .line = 2, .index = 13, .path = "test-examples/1.cb"}, {.type = Fn, .data = 0, .line = 4, .index = 0, .path = "test-examples/1.cb"}, {.type = Id, .data = "main", .line = 4, .index = 3, .path = "test-examples/1.cb"}, {.type = LBra, .data = 0, .line = 4, .index = 7, .path = "test-examples/1.cb"}, {.type = RBra, .data = 0, .line = 4, .index = 8, .path = "test-examples/1.cb"}, {.type = Min, .data = 0, .line = 4, .index = 10, .path = "test-examples/1.cb"}, {.type = GtGt, .data = 0, .line = 4, .index = 11, .path = "test-examples/1.cb"}, {.type = I32, .data = 0, .line = 4, .index = 13, .path = "test-examples/1.cb"}, {.type = LCur, .data = 0, .line = 4, .index = 17, .path = "test-examples/1.cb"}, {.type = Ret, .data = 0, .line = 5, .index = 4, .path = "test-examples/1.cb"}, {.type = IntLit, .data = "69", .line = 5, .index = 11, .path = "test-examples/1.cb"}, {.type = DotComma, .data = 0, .line = 5, .index = 13, .path = "test-examples/1.cb"}, {.type = RCur, .data = 0, .line = 6, .index = 0, .path = "test-examples/1.cb"}
    };

    LexerTokenise(&l, &cu, &a);

    for(int i = 0; i < l.size; i++) {
        Token * a = l.tokens + i;
        Token * b = tokens + i;

        assert(a->type == b->type);

        if(a->data == 0) assert(b->data == 0);
        else assert(strcmp(a->data, b->data) == 0);
        assert(a->index == b->index);
        assert(a->line == b->line);
        assert(strcmp(a->path, b->path) == 0);
    }

    printf("Test success\n");
}

int main() {
    test_lexer();
}