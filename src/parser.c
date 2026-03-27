#define _GNU_SOURCE
#include "ast.h"
#include "lexer.h"
#include "util.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "parser.h"

#define PARSER_TODO fprintf(stderr, "TODO triggered in %s\n", __FUNCTION__); exit(EXIT_FAILURE); return 0;

// Terminates on failure
AstNode * _SafeAstNodeNew(allocator_t *a, AstType type, const char * function) {
    AstNode * result = AstNodeNew(a, type);
    if(!result) {
        terminate("Allocation error in %s\n", function);
        return 0;
    }
    return result;
}

// Init the parser to default values
void ParserInit(Parser *self) {
    self->index = 0;
    self->root = 0;
    DA_INIT(&self->inserted_tokens);
    DA_INIT(&self->errors);
}

// Add an error
void _ParserError(Parser * self, Lexer * l, Token * t, allocator_t * a, const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    char * inner_msg = 0;
    vasprintf(&inner_msg, fmt, ap);

    char * outer_msg = 0;
    asprintf(&outer_msg, "Wrong token found at %ld:%ld: %s\n", t->line, t->index, inner_msg);

    free(inner_msg);

    DA_PUSHBACK(&self->errors, outer_msg, a);

    va_end(ap);
}

// Peek in the token stream, the position is calculates as current_location + offset
// Terminate if the token stream is ended
Token * _ParserPeek(Parser * self, Lexer * l, size_t offset) {
    if((self->index + offset) >= l->tokens.size) {
        terminate("Parser reached end!\n");
    }
    return &l->tokens.data[self->index + offset];
    //return da_Token_get_ref(&l->tokens, self->index + offset);
}

// Eat a token (advance the position in the token stream) and return the token
// Return 0 if the token to eat does not match
Token * _ParserEat(Parser * self, Lexer * l, TokenType type) {
    Token * t = &l->tokens.data[self->index];
    //Token * t = da_Token_get_ref(&l->tokens, self->index);
    if(t->type != type) {
        return 0;
    }
    self->index++;
    return t;
}

// Does not consume the found synchronisation token
TokenType _ParserSyncTo(Parser * self, Lexer * l, TokenType * sync_tokens, size_t sync_tokens_count) {
    while(1) {
        Token * t = _ParserPeek(self, l, 0);
        for(size_t i = 0; i < sync_tokens_count; i++) {
            if(t->type == sync_tokens[i]) {
                return t->type;
            }
        }
        _ParserEat(self, l, t->type);
    }
}

// simple_type -> "i8" | "i16" | "i32" | "i64" | "isize" | "u8" | "u16" | "u32" | "u64" | "usize" | "bool" | "void"
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
        case L_Void: _ParserEat(self, l, L_Void); break;
        default: return 0; break;
    }

    AstNode * result = _SafeAstNodeNew(a, AstType_DataType, __FUNCTION__);
    result->data_type.kind = AstDataType_Simple;
    result->data_type.data_type.simple_type = t->type;

    return result;
}

// base_type -> simple_type | id
AstNode * _ParserParseBaseType(Parser * self, Lexer * l, allocator_t * a) {
    AstNode * result = _ParserParseSimpleType(self, l, a);
    if(!result) {
        Token * t = _ParserEat(self, l, L_Id);
        if(t) {
            result = _SafeAstNodeNew(a, AstType_DataType, __FUNCTION__);
            result->data_type.kind = AstDataType_Complex;
            result->data_type.data_type.complex_type = (char *)t->data;
        }
    }

    return result;
}

// complex_type -> base_type pointer_suffix
// pointer_suffix -> "*" pointer_suffix | e
AstNode * _ParserParseComplexType(Parser * self, Lexer * l, allocator_t * a) {
    AstNode * result = _ParserParseBaseType(self, l, a);
    if(result) {
        while(_ParserEat(self, l, L_Star)) {
            result->data_type.stars += 1;
        }
    }

    return result;
}

AstNode * _ParserParseExpression(Parser * self, Lexer * l, allocator_t * a) {
    PARSER_TODO
}

// variable_declaration_expression -> "let" id (":" complex_type | e) "=" expression
// Note: will need forward definition because this is used in expression
AstNode * _ParserParseVariableDeclarationExpression(Parser * self, Lexer * l, allocator_t * a) {
    if(!_ParserEat(self, l, L_Let)) return 0;

    // Error handling should start here

    AstNode * result = _SafeAstNodeNew(a, AstType_DeclExp, __FUNCTION__);

    Token * t = _ParserEat(self, l, L_Id);
    char * id_data = 0;
    if(!t) {
        t = _ParserPeek(self, l, 0);
        _ParserError(self, l, t, a, "expected 'identifier'");
        TokenType sync_tokens[] = {L_Colon, L_Eq, L_DotComma};
        _ParserSyncTo(self, l, sync_tokens, sizeof(sync_tokens) / sizeof(sync_tokens[0]));
    } else id_data = (char *)t->data;

    AstNode * complex_type = 0;

    if(_ParserEat(self, l, L_Colon)) {
        complex_type = _ParserParseComplexType(self, l, a);
    }

    if(!_ParserEat(self, l, L_Eq)) {
        t = _ParserPeek(self, l, 0);
        _ParserError(self, l, t, a, "expected '='");
        TokenType sync_tokens[] = {L_DotComma};
        _ParserSyncTo(self, l, sync_tokens, sizeof(sync_tokens) / sizeof(sync_tokens[0]));
    };

    AstNode * expression = _ParserParseExpression(self, l, a);

    result->decl_exp.id = (char *)id_data;
    result->decl_exp.data_type = complex_type;
    result->decl_exp.expression = expression;

    return result;
}

// function_call_expression_argument_list -> expression function_call_expression_argument_list_tail | e
// function_call_expression_argument_list_tail -> "," expression function_call_expression_argument_list_tail | e
int _ParserParseFunctionCallExpressionArgumentList(Parser * self, Lexer * l, da_AstNode * list, allocator_t * a) {
    AstNode * expression = _ParserParseExpression(self, l, a);
    if(!expression) return 0;
    // TODO: I think this can be a do-while (27/03/2026 @ 22:13)
    DA_PUSHBACK(list, expression, a);
    //da_AstNode_pushback(list, &expression, a);

    while(_ParserEat(self, l, L_Comma)) {
        expression = _ParserParseExpression(self, l, a);
        if(!expression) return 0;
        DA_PUSHBACK(list, expression, a);
        //da_AstNode_pushback(list, &expression, a);
    }

    return 1;
}

// function_call_expression -> id "(" function_call_expression_argument_list ")"
AstNode * _ParserParseFunctionCallExpression(Parser * self, Lexer * l, allocator_t * a) {
    Token * id = _ParserEat(self, l, L_Id);
    if(!id) return 0;

    if(!_ParserEat(self, l, L_LBra)) {
        Token * t = _ParserPeek(self, l, 0);
        _ParserError(self, l, t, a, "expected '('");
        TokenType sync_tokens[] = {L_LBra, L_DotComma};
        _ParserSyncTo(self, l, sync_tokens, sizeof(sync_tokens) / sizeof(sync_tokens[0]));
        if(!_ParserEat(self, l, L_LBra)) return 0;
    };

    da_AstNode args;
    DA_INIT(&args);
    //da_AstNode_new(&args);
    _ParserParseFunctionCallExpressionArgumentList(self, l, &args, a);

    if(!_ParserEat(self, l, L_RBra)) {
        Token * t = _ParserPeek(self, l, 0);
        _ParserError(self, l, t, a, "expected ')'");
        TokenType sync_tokens[] = {L_LBra, L_DotComma};
        _ParserSyncTo(self, l, sync_tokens, sizeof(sync_tokens) / sizeof(sync_tokens[0]));
        if(!_ParserEat(self, l, L_RBra)) return 0;
    }

    AstNode * result = _SafeAstNodeNew(a, AstType_FuncCallExp, __FUNCTION__);
    result->func_call_exp.id = (char *)id->data;
    result->func_call_exp.arguments = args;

    return result;
}

void ParserParse(Parser * self, Lexer * l, allocator_t * a) {
    self->root = _ParserParseVariableDeclarationExpression(self, l, a);
    assert(self->root != NULL);
}