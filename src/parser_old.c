#define MDA_IMPLEMENTATION
#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "util.h"
#include <assert.h>
#include <stdint.h>

/*
Note: perhaps there is no need to check if returned nodes are nullpointers because if they are null previous functions have already registered an error
*/

// Terminates on failure
AstNode * _SafeAstNodeNew(allocator_t *a, AstType type, char * function) {
    AstNode * result = AstNodeNew(a, type);
    if(!result) {
        terminate("Allocation error in %s\n", function);
        return 0;
    }
    return result;
}

void ParserInit(Parser *self);
void _ParserError(Parser * self, Lexer * l, TokenType expected, allocator_t * a);
Token * _ParserPeek(Parser * self, Lexer * l, size_t offset);
Token * _ParserEat(Parser * self, Lexer * l, TokenType type);
AstNode * _ParserParseSimpleType(Parser * self, Lexer * l, allocator_t * a);
AstNode * _ParserParseComplexType(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionSignatureReturnType(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionSignatureParameter(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionSignatureParameterListTail(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionSignatureParameterList(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionSignature(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseBody(Parser * self, Lexer * l, allocator_t * a);
AstNode * _ParserParseExpression(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseVariableInitialisation(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseVariableExpression(Parser * self, Lexer * l, allocator_t * a);
AstNode * _ParserParseVariableAssignment(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionCallParameterListTail(Parser * self, Lexer * l, allocator_t * a, da_AstNode * args);
int _ParserParseFunctionCallParameterList(Parser * self, Lexer * l, allocator_t * a, da_AstNode * args);
AstNode * _ParserParseFunctionCall(Parser * self, Lexer * l, allocator_t * a);
int _ParserParseFunctionImplementation(Parser * self, Lexer * l, allocator_t * a);
void ParserParse(Parser *self, Lexer *l, allocator_t *a);

void ParserInit(Parser *self) {
    self->index = 0;
    da_Token_new(&self->inserted_tokens);
    da_ParserError_new(&self->errors);
}

void _ParserError(Parser * self, Lexer * l, TokenType expected, allocator_t * a) {
    Token * got = _ParserPeek(self, l, 0);
    ParserError error = {.got = got, .expected = expected};
    da_ParserError_pushback(&self->errors, &error, a);
}

Token * _ParserPeek(Parser * self, Lexer * l, size_t offset) {
    if((self->index + offset) >= l->tokens.size) {
        terminate("Parser reached end!\n");
    }
    return da_Token_get_ref(&l->tokens, self->index + offset);
}

Token * _ParserEat(Parser * self, Lexer * l, TokenType type) {
    Token * t = da_Token_get_ref(&l->tokens, self->index);
    if(t->type != type) {
        return 0;
    }
    self->index++;
    return t;
}

AstNode * _ParserParseSimpleType(Parser * self, Lexer * l, allocator_t * a) {
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
        default: return 0; break;
    }

    AstNode * simple_type = _SafeAstNodeNew(a, AstType_DataType, __FUNCTION__);
    simple_type->data.data_type.kind = AstDataType_Simple;
    simple_type->data.data_type.type.simple_type = t->type;
    simple_type->data.data_type.stars = 0;
    return simple_type;
}

// TODO: pointers and void (but void can only be returned from a function, but void * can also be returned from a function)
AstNode * _ParserParseComplexType(Parser * self, Lexer * l, allocator_t * a) {
    AstNode * result = 0;

    // void *...
    Token * t = _ParserPeek(self, l, 0);
    if(t->type == L_Void) {
        t = _ParserPeek(self, l, 1);
        if(t->type != L_Star) return result;

        result = _SafeAstNodeNew(a, AstType_DataType, __FUNCTION__);

        result->data.data_type.kind = AstDataType_Simple;
        result->data.data_type.type.simple_type = L_Void;

        _ParserEat(self, l, L_Void); // void
        
        while(_ParserEat(self, l, L_Star)) {
            result->data.data_type.stars += 1;
        }

        return result;
    }

    // simple_type *...
    result = _ParserParseSimpleType(self, l, a);

    if(!result) {
        // custom_type *...
        t = _ParserEat(self, l, L_Id);
        if(t) {
            result = _SafeAstNodeNew(a, AstType_DataType, __FUNCTION__);
            result->data.data_type.kind = AstDataType_Complex;
            result->data.data_type.type.complex_type = (char *)t->data;
        }
    }

    if(result) {
        while(_ParserEat(self, l, L_Star)) {
            result->data.data_type.stars += 1;
        }
    }

    return result;
}

AstNode * _ParserParseFunctionSignatureReturnType(Parser * self, Lexer * l, allocator_t * a) {
    Token * t = _ParserPeek(self, l, 0);
    if(t->type == L_Void) {
        t = _ParserPeek(self, l, 1);
        if(t->type != L_Star) {
            _ParserEat(self, l, L_Void);
            return _SafeAstNodeNew(a, AstType_DataType, __FUNCTION__); // void
        }
    }

    // Everything else
    return _ParserParseComplexType(self, l, a);
}

// id : complex_type
AstNode * _ParserParseFunctionSignatureParameter(Parser * self, Lexer * l, allocator_t * a) {
    Token * id = _ParserEat(self, l, L_Id);
    if(!id) return 0;

    Token *  t = _ParserEat(self, l, L_Colon);
    if(!t) {
        t = _ParserPeek(self, l, 0);
        ParserError pe = {.got = t, .expected = L_Colon};
        da_ParserError_pushback(&self->errors, &pe, a);
        _ParserEat(self, l, t->type);
    }

    AstNode * complex_type = _ParserParseComplexType(self, l, a);
    if(!complex_type) return 0;

    AstNode * result = _SafeAstNodeNew(a, AstType_)
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
    return _ParserParseFunctionSignatureReturnType(self, l, a);
}

int _ParserParseBody(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

AstNode * _ParserParseExpression(Parser * self, Lexer * l, allocator_t * a) {
    return 0;
}

/*int _ParserParseVariableInitialisation(Parser * self, Lexer * l, allocator_t * a) {
    if(!_ParserEat(self, l, L_Let)) return 0;
    Token * name = _ParserEat(self, l, L_Id);
    if(!name) return 0;
    if(!_ParserEat(self, l, L_Colon)) return 0;
    Token * type = _ParserParseComplexType(self, l, a); // TODO: stopped here 
    if(!result) return 0;
    result = _ParserEat(self, l, L_Eq);
    if(!result) return 0;
    result = _ParserParseExpression(self, l, a);
    return result;
}*/

// IDK what this does here
int _ParserParseVariableExpression(Parser * self, Lexer * l, allocator_t * a) {
    int result = _ParserEat(self, l, L_Id);
    if(!result) return 0;
    result = _ParserEat(self, l, L_Eq);
    if(!result) return 0;
    result = _ParserParseExpression(self, l, a);
    return result;
}

AstNode * _ParserParseVariableAssignment(Parser * self, Lexer * l, allocator_t * a) {
    Token * name = _ParserEat(self, l, L_Id);
    if(!name) {
        _ParserError(self, l, L_Id, a);
    }

    if(!_ParserEat(self, l, L_Eq)) {
        _ParserError(self, l, L_Eq, a);
    }

    AstNode * expression = _ParserParseExpression(self, l, a);

    if(!expression) {
        terminate("Unrecoverable error in _ParserParseVariableAssignment");
    }

    AstNode * variable_assignment = _SafeAstNodeNew(a, AstType_AssignStm, __FUNCTION__);
    variable_assignment->data.assign_stm.name = (char *)name->data;
    variable_assignment->data.assign_stm.expression = expression;

    return variable_assignment;
}

int _ParserParseFunctionCallParameterListTail(Parser * self, Lexer * l, allocator_t * a, da_AstNode * args) {
    // If no comma eaten, 0 is returned, !0 = 1
    if(!_ParserEat(self, l, L_Comma)) return 1;

    AstNode * arg = _ParserParseExpression(self, l, a);
    if(!arg) {
        terminate("Unrecoverable error in _ParserParseFunctionCallParameterListTail");
    }

    da_AstNode_pushback(args, &arg, a);

    return _ParserParseFunctionCallParameterListTail(self, l, a, args);
}

int _ParserParseFunctionCallParameterList(Parser * self, Lexer * l, allocator_t * a, da_AstNode * args) {
    AstNode * arg = _ParserParseExpression(self, l, a);
    if(!arg) return 1; // No arguments passed
    da_AstNode_pushback(args, &arg, a);

    int result = _ParserParseFunctionCallParameterListTail(self, l, a, args);
    return result;
}

AstNode * _ParserParseFunctionCall(Parser * self, Lexer * l, allocator_t * a) {
    AstNode * function_call = _SafeAstNodeNew(a, AstType_FuncCallExp, __FUNCTION__);
    Token * name = _ParserEat(self, l, L_Id);
    if(!name) {
        _ParserError(self, l, L_Id, a);
    }

    function_call->data.func_call_exp.name = (char *)name->data;

    if(!_ParserEat(self, l, L_LBra)) {
        _ParserError(self, l, L_LBra, a);
    }

    // oldWARN: this is a quick workaround because the vector implementation I am using does not work right, capping the max amount of function call parameters to 20, which is plenty
    // This is solved the same day, we are no weak ass programming b*tch
    da_AstNode_new(&function_call->data.func_call_exp.args);
    if(!_ParserParseFunctionCallParameterList(self, l, a, &function_call->data.func_call_exp.args)) {
        // TODO: another error
        terminate("Unrecoverable error in _ParserParseFunctionCall");
    }

    if(!_ParserEat(self, l, L_RBra)) {
        _ParserError(self, l, L_RBra, a);
    };

    return function_call;
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
