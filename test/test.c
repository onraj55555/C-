#include "compilation_unit.h"
#include "lexer.h"
#include <assert.h>
#include <string.h>
#define ALLOCATOR_HEAP_ALLOCATOR
#include "allocator.h"

#define TEST_FILE_1 "test-examples/1.cb"

void test_lexer() {
    CompilationUnit cu;
    CompilationUnitFromFile(&cu, TEST_FILE_1);

    Lexer l;
    LexerNew(&l);

    allocator_t a;
    allocator_new_heap_allocator(&a);

    Token tokens[] = {
        {.type = Pack, .data = 0, .line = 0, .index = 0, .path = TEST_FILE_1}, {.type = Id, .data = "app", .line = 0, .index = 8, .path = TEST_FILE_1}, {.type = DotComma, .data = 0, .line = 0, .index = 11, .path = TEST_FILE_1}, {.type = Import, .data = 0, .line = 2, .index = 0, .path = TEST_FILE_1}, {.type = LBra, .data = 0, .line = 2, .index = 6, .path = TEST_FILE_1}, {.type = StringLit, .data = "std", .line = 2, .index = 7, .path = TEST_FILE_1}, {.type = RBra, .data = 0, .line = 2, .index = 12, .path = TEST_FILE_1}, {.type = DotComma, .data = 0, .line = 2, .index = 13, .path = TEST_FILE_1}, {.type = Fn, .data = 0, .line = 4, .index = 0, .path = TEST_FILE_1}, {.type = Id, .data = "main", .line = 4, .index = 3, .path = TEST_FILE_1}, {.type = LBra, .data = 0, .line = 4, .index = 7, .path = TEST_FILE_1}, {.type = RBra, .data = 0, .line = 4, .index = 8, .path = TEST_FILE_1}, {.type = Min, .data = 0, .line = 4, .index = 10, .path = TEST_FILE_1}, {.type = GtGt, .data = 0, .line = 4, .index = 11, .path = TEST_FILE_1}, {.type = I32, .data = 0, .line = 4, .index = 13, .path = TEST_FILE_1}, {.type = LCur, .data = 0, .line = 4, .index = 17, .path = TEST_FILE_1}, {.type = Ret, .data = 0, .line = 5, .index = 4, .path = TEST_FILE_1}, {.type = IntLit, .data = "69", .line = 5, .index = 11, .path = TEST_FILE_1}, {.type = DotComma, .data = 0, .line = 5, .index = 13, .path = TEST_FILE_1}, {.type = RCur, .data = 0, .line = 6, .index = 0, .path = TEST_FILE_1}
    };

    LexerTokenise(&l, &cu, &a);

    for(int i = 0; i < l.size; i++) {
        Token * a = vector_Token_get_ref(&l.tokens, i);
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
