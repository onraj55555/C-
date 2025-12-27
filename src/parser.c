#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "util.h"
#include <assert.h>
#include <stdint.h>

void ParserInit(Parser *self) {
    self->index = 0;
}

Token * _ParserPeek(Parser * self, Lexer * l, size_t offset) {
    if((self->index + offset) >= l->tokens.size) {
        terminate("Parser reached end!\n");
    }
    return da_Token_get_ref(&l->tokens, self->index + offset);
}

void _ParserError(Parser * self) {
    terminate("Parser terminated at index %d\n", self->index);
}

Token * _ParserEat(Parser * self, Lexer * l, TokenType type) {
    Token * t = da_Token_get_ref(&l->tokens, self->index);
    if(t->type != type) {
        return 0;
    }
    self->index++;
    return t;
}

int _ParserParseSimpleType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    switch(t->type) {
        case L_I8: _ParserEat(self, l, L_I8); break;
        case L_I16: _ParserEat(self, l, L_I16); break;
        case L_I32: _ParserEat(self, l, L_I32); break;
        case L_I64: _ParserEat(self, l, L_I64); break;
        case L_U8: _ParserEat(self, l, L_U8); break;
        case L_U16: _ParserEat(self, l, L_U16); break;
        case L_U32: _ParserEat(self, l, L_U32); break;
        case L_U64: _ParserEat(self, l, L_U64); break;
        case L_Bool: _ParserEat(self, l, L_Bool); break;
        case L_USize: _ParserEat(self, l, L_USize); break;
        case L_ISize: _ParserEat(self, l, L_ISize); break;
        default: return 0;
    }
    return 1;
}

// TODO: pointers and void (but void can only be returned from a function, but void * can also be returned from a function)
int _ParserParseComplexType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type == L_Void) {
        t = _ParserPeek(self, l, 1);
        if(t->type != L_Star) return 0;
        _ParserEat(self, l, L_Void); // void
        _ParserEat(self, l, L_Star); // *
        while(_ParserEat(self, l, L_Star)); // ...*
        return 1;
    }
    int result = _ParserParseSimpleType(self, l, a);
    if(!result) result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    while(_ParserEat(self, l, L_Star));
    return 1;
}

int _ParserParseFuntionSignatureReturnType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type == L_Void) {
        t = _ParserPeek(self, l, 1);
        if(t->type != L_Star) {
            return _ParserEat(self, l, L_Void);
        }
    }
    return _ParserParseComplexType(self, l, a);
}

int _ParserParseFunctionSignatureParameter(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Colon);
    if(!result) return 0;
    return _ParserParseComplexType(self, l, a);
}

int _ParserParseFunctionSignatureParameterListTail(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type != L_Comma) return 1;
    _ParserEat(self, l, L_Comma);
    int result = _ParserParseFunctionSignatureParameter(self, l, a);
    if(!result) return 0;
    return _ParserParseFunctionSignatureParameterListTail(self, l, a);
}

int _ParserParseFunctionSignatureParameterList(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type != L_Id) return 1;
    int result = _ParserParseFunctionSignatureParameter(self, l, a);
    if(!result) return 0;
    return _ParserParseFunctionSignatureParameterListTail(self, l, a);
}

int _ParserParseFunctionSignature(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, L_Fn);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    result = _ParserEat(self, l, L_LBra);
    if(!result) return 0;
    result = _ParserParseFunctionSignatureParameterList(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, L_RBra);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Arrow);
    if(!result) return 0;
    return _ParserParseFuntionSignatureReturnType(self, l, a);
}

int _ParserParseBody(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

AstNode _ParserParseExpression(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

int _ParserParseVariableInitialisation(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, L_Let);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Colon);
    if(!result) return 0;
    result = _ParserParseComplexType(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Eq);
    if(!result) return 0;
    result = _ParserParseExpression(self, l, a);
    return result;
}

int _ParserParseVariableExpression(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Eq);
    if(!result) return 0;
    result = _ParserParseExpression(self, l, a);
    return result;
}

int _ParserParseVariableAssignment(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Eq);
    if(!result) return 0;
    result = _ParserParseExpression(self, l, a);
    return result;
}

int _ParserParseFunctionCallParameterListTail(Parser * self, Lexer * l, allocator_t * a, da_AstNode * args) {
    if(!_ParserEat(self, l, L_Comma)) return 1;

    AstNode arg = _ParserParseExpression(self, l, a);
    if(!arg) {} // TODO: handle error

    da_AstNode_pushback(args, &arg, a);

    return _ParserParseFunctionCallParameterListTail(self, l, a);
}

int _ParserParseFunctionCallParameterList(Parser * self, Lexer * l, allocator_t * a, da_AstNode * args) {
    AstNode arg = _ParserParseExpression(self, l, a);
    if(!arg) return 1; // No arguments passed
    da_AstNode_pushback(args, &arg, a);

    int result = _ParserParseFunctionCallParameterListTail(self, l, a, da_AstNode * args);
    return result;
}

int _ParserParseFunctionCall(Parser * self, Lexer * l, allocator_t * a) {
    AstNode function_call = AstNodeNew(a, A_FuncCallExp);
    Token * result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    function_call->data.func_call_exp.name = (char *)result->data;

    result = _ParserEat(self, l, L_LBra);
    if(!result) return 0;

    // oldWARN: this is a quick workaround because the vector implementation I am using does not work right, capping the max amount of function call parameters to 20, which is plenty
    // This is solved the same day, we are no weak ass programming b*tch
    da_AstNode args;
    da_AstNode_new(&args);
    result = _ParserParseFunctionCallParameterList(self, l, a, &args);

    if(!result) return 0;
    result = _ParserEat(self, l, L_RBra);
    return result;
}

int _ParserParseFunctionImplementation(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserParseFunctionSignature(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, L_LCur);
    if(!result) return 0;
    result = _ParserParseBody(self, l, a);
    if(!result) return 0;
    result = _ParserEat(self, l, L_RCur);
    if(!result) return 0;
    return 1;
}

void ParserParse(Parser *self, Lexer *l, allocator_t *a) {
    int result = _ParserParseFunctionSignature(self, l, a);
    if(!result) printf("Could not parse!\n");
}
