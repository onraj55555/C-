#define MDA_IMPLEMENTATION
#include "ast.h"

AstNode AstNodeNew(allocator_t *a, AstType type) {
    AstNode node = allocator_alloc(a, sizeof(AstNode));
    node->type = type;
    return node;
}
