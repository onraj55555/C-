#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include "allocator.h"
#include "ast.h"

typedef struct Parser Parser;

struct Parser {
    size_t index;
};

void ParserInit(Parser * self);
void ParserParse(Parser * self, Lexer * l, allocator_t * a);

#endif
