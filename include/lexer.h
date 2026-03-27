#ifndef LEXER_H_
#define LEXER_H_

#include "allocator.h"
#include "compilation_unit.h"
#include <stdint.h>
#include "dynamic_array.h"

typedef enum {
    L_Id, // identifier
    L_IntLit, // 69
    L_FloatLit, // 3.14
    L_StringLit, // "abc"
    L_Fn, // fn
    L_Colon, // :
    L_Comma, // ,
    L_LBra, // (
    L_RBra, // )
    L_LCur, // {
    L_RCur, // }
    L_LAng, // [
    L_RAng, // ]
    L_Arrow, // ->
    L_Plus, // +
    L_Min, // -
    L_Star, // *
    L_Slash, // /
    L_Perc, // %
    L_Dot, // .
    L_And, // &
    L_Or, // |
    L_AndAnd, // &&
    L_OrOr, // ||
    L_LtLt, // <<
    L_GtGt, // >>
    L_Eq, // =
    L_EqEq, // ==
    L_Lt, // <
    L_Gt, // >
    L_LtEq, // <=
    L_GtEq, // >=
    L_Bang, // !
    L_BangEq, // !=
    L_If, // if
    L_Else, // else
    L_While, // while
    L_For, // for
    L_Ret, // return
    L_Struct, // struct
    L_I8, // i8
    L_I16, // i16
    L_I32, // i32
    L_I64, // i64
    L_U8, // u8
    L_U16, // u16
    L_U32, // u32
    L_U64, // u64
    L_F32, // f32
    L_F64, // f64
    L_Bool, // bool
    L_Void, // void
    L_Hash, // #
    L_USize, // usize
    L_ISize, // isize
    L_DotComma, // ;
    L_Mod, // mod
    L_Use, // use
    L_Let, // let
    L_EOF, // end if the unput
} TokenType;

typedef struct {
    TokenType type;
    void * data;
    uint64_t line;
    uint64_t index;
    char * path;
} Token;

typedef DA_CREATE_STRUCT(da_Token, Token) da_Token;

typedef struct {
    int has_error;
    da_Token tokens;
} Lexer;

void LexerNew(Lexer * self);
void LexerTokenise(Lexer * self, CompilationUnit * cu, allocator_t * a);
void LexerPrint(Lexer * self);
void LexerPrintInitialisation(Lexer * self);

#endif
