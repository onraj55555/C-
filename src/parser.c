#include "parser.h"
#include "lexer.h"
#include "util.h"

void ParserInit(Parser *self) {
    self->index = 0;
}

Token * _ParserPeek(Parser * self, Lexer * l, size_t offset) {
    if((self->index + offset) >= l->tokens.size) {
        terminate("Parser reached end!\n");
    }
    return vector_Token_get_ref(&l->tokens, self->index + offset);
}

void _ParserError(Parser * self) {
    terminate("Parser terminated at index %d\n", self->index);
}

int _ParserEat(Parser * self, Lexer * l, TokenType type) {
    Token * t = vector_Token_get_ref(&l->tokens, self->index);
    if(t->type != type) {
        //_ParserError(self);
        return 0;
    }
    self->index++;
    return 1;
}

int _ParserParseSimpleType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    switch(t->type) {
        case I8: _ParserEat(self, l, I8); break;
        case I16: _ParserEat(self, l, I16); break;
        case I32: _ParserEat(self, l, I32); break;
        case I64: _ParserEat(self, l, I64); break;
        case U8: _ParserEat(self, l, U8); break;
        case U16: _ParserEat(self, l, U16); break;
        case U32: _ParserEat(self, l, U32); break;
        case U64: _ParserEat(self, l, U64); break;
        case Bool: _ParserEat(self, l, Bool); break;
        case USize: _ParserEat(self, l, USize); break;
        case ISize: _ParserEat(self, l, ISize); break;
        default: return 0;
    }
    return 1;
}

// TODO: pointers and void (but void can only be returned from a function, but void * can also be returned from a function)
int _ParserParseComplexType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type == Void) {
        t = _ParserPeek(self, l, 1);
        if(t->type != Star) return 0;
        _ParserEat(self, l, Void); // void
        _ParserEat(self, l, Star); // *
        while(_ParserEat(self, l, Star)); // ...*
        return 1;
    }
    int result = _ParserParseSimpleType(self, l, a);
    if(!result) result = _ParserEat(self, l, Id);
    if(!result) return 0;
    while(_ParserEat(self, l, Star));
    return 1;
}

int _ParserParseFuntionSignatureReturnType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type == Void) {
        t = _ParserPeek(self, l, 1);
        if(t->type != Star) {
            return _ParserEat(self, l, Void);
        }
    }
    return _ParserParseComplexType(self, l, a);
}

int _ParserParseFunctionSignatureParameter(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, Id);
    if(!result) return 0;
    result = _ParserEat(self, l, Colon);
    if(!result) return 0;
    return _ParserParseComplexType(self, l, a);
}

int _ParserParseFunctionSignatureParameterListTail(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type != Comma) return 1;
    _ParserEat(self, l, Comma);
    int result = _ParserParseFunctionSignatureParameter(self, l, a);
    if(!result) return 0;
    return _ParserParseFunctionSignatureParameterListTail(self, l, a);
}

int _ParserParseFunctionSignatureParameterList(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type != Id) return 1;
    int result = _ParserParseFunctionSignatureParameter(self, l, a);
    if(!result) return 0;
    return _ParserParseFunctionSignatureParameterListTail(self, l, a);
}

int _ParserParseFunctionSignature(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, Fn);
    if(!result) return 0;
    result = _ParserEat(self, l, Id);
    if(!result) return 0;
    result = _ParserEat(self, l, LBra);
    if(!result) return 0;
    result = _ParserParseFunctionSignatureParameterList(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, RBra);
    if(!result) return 0;
    result = _ParserEat(self, l, Arrow);
    if(!result) return 0;
    return _ParserParseFuntionSignatureReturnType(self, l, a);
}

int _ParserParseBody(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

int _ParseExpression(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

int _ParserParseVariableInitialisation(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

int _ParserParseFunctionImplementation(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserParseFunctionSignature(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, LCur);
    if(!result) return 0;
    result = _ParserParseBody(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, RCur);
    if(!result) return 0;
    return 1;
}

void ParserParse(Parser *self, Lexer *l, allocator_t *a) {
    int result = _ParserParseFunctionSignature(self, l, a);
    if(!result) printf("Could not parse!\n");
}
