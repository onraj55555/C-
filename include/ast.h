#ifndef AST_H_
#define AST_H_

#include "allocator.h"
#include <stdint.h>
#include "lexer.h"
#include "dynamic_array.h"

typedef union AstNode AstNode;

typedef DA_CREATE_STRUCT(da_AstNode, AstNode *) da_AstNode;

typedef enum {
    AstType_CompStm,
    AstType_AssignStm,
    AstType_DeclExp,
    AstType_BinOpExp,
    AstType_LitExp, // Any literal, might change thin later to int, float and string
    AstType_IdExp, // Any identifier
    AstType_FuncCallExp,
    AstType_DataType,
} AstType;

/*union AstData {
    struct AstCompStmNode {
        AstNode * first;
        AstNode * last;
    } comp_stm;

    struct AstAssignStm {
        char * name;
        AstNode * expression;
    } assign_stm;

    struct AstBinOpExp {
        enum {
            AstBinOpExp_Plus,
            AstBinOpExp_Min,
            AstBinOpExp_Mul,
            AstBinOpExp_Div,
            AstBinOpExp_Mod
        } op;
        AstNode * left;
        AstNode * right;
    } bin_op_exp;

    struct AstIdExp {
        char * name;
    } id_exp;

    struct AstLitExp {
        char * lit;
    } lit_exp;

    struct AstFuncCallExp {
        char * name;
        da_AstNode args;
    } func_call_exp;

    struct AstDataType {
        enum {
            AstDataType_Simple,
            AstDataType_Complex
        } kind;
        union type {
            TokenType simple_type;
            char * complex_type;
        } type;
        uint64_t stars;
    } data_type;

    struct AstFuncSignature
};*/

typedef struct AstDataType {
    AstType type;
    enum {
        AstDataType_Simple,
        AstDataType_Complex
    } kind;
    union type {
        TokenType simple_type;
        char * complex_type;
    } data_type;
    uint64_t stars;
} AstDataType;

typedef struct AstDeclExp {
    AstType type;
    char * id;
    AstNode * data_type;
    AstNode * expression;
} AstDeclExp;

typedef struct AstFuncCallExp {
    AstType type;
    char * id;
    da_AstNode arguments;
} AstFuncCallExp;

union AstNode {
    AstType type;
    AstDataType data_type;
    AstDeclExp decl_exp;
    AstFuncCallExp func_call_exp;
};

AstNode * AstNodeNew(allocator_t * a, AstType type);

#endif
