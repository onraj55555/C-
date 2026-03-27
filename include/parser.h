#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include "allocator.h"
#include "ast.h"
#include "da_string.h"

typedef struct Parser Parser;

struct Parser {
    size_t index;
    AstNode * root;
    da_Token inserted_tokens;
    da_string errors;
};

void ParserInit(Parser * self);
void ParserParse(Parser * self, Lexer * l, allocator_t * a);

#endif
