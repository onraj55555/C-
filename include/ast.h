#ifndef AST_H_
#define AST_H_

#include "allocator.h"
#include <stdint.h>

typedef struct AstNode_ * AstNode;
typedef union AstData AstData;

#define MDA_TYPE AstNode
#include "mda.h"

typedef enum {
    A_CompStm,
    A_AssignStm,
    A_BinOpExp,
    A_LitExp, // Any literal, might change thin later to int, float and string
    A_IdExp, // Any identifier
    A_FuncCallExp
} AstType;

union AstData {
    struct AstCompStmNode {
        AstNode * first;
        AstNode * last;
    } comp_stm;

    struct AstAssignStm {
        AstNode * left;
        AstNode * right;
    } assign_stm;

    struct AstBinOpExp {
        enum {
            A_BIN_OP_Plus,
            A_BIN_OP_Min,
            A_BIN_OP_Mul,
            A_BIN_OP_Div,
            A_BIN_OP_Mod
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
};

struct AstNode_ {
    AstType type;
    AstData data;
};

AstNode AstNodeNew(allocator_t * a, AstType type);

#endif
