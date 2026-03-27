#include "ast.h"

// Allocates an AST node (0 initialised)
AstNode * AstNodeNew(allocator_t *a, AstType type) {
    AstNode * node = allocator_clean_alloc(a, 1, sizeof(AstNode));
    node->type = type;
    return node;
}
