#ifndef LEXER_H_
#define LEXER_H_

#include "compilation_unit.h"
#include <stdint.h>
#include "allocator.h"

typedef enum {
    Id, // identifier
    IntLit, // 69
    FloatLit, // 3.14
    StringLit, // "abc"
    Fn, // fn
    Colon, // :
    Comma, // ,
    LBra, // (
    RBra, // )
    LCur, // {
    RCur, // }
    LAng, // [
    RAng, // ]
    Arrow, // ->
    Plus, // +
    Min, // -
    Star, // *
    Slash, // /
    Perc, // %
    Dot, // .
    And, // &
    Or, // |
    AndAnd, // &&
    OrOr, // ||
    LtLt, // <<
    GtGt, // >>
    Eq, // =
    EqEq, // ==
    Lt, // <
    Gt, // >
    LtEq, // <=
    GtEq, // >=
    Bang, // !
    BangEq, // !=
    If, // if
    Else, // else
    While, // while
    For, // for
    Ret, // return
    Struct, // struct
    I8, // i8
    I16, // i16
    I32, // i32
    I64, // i64
    U8, // u8
    U16, // u16
    U32, // u32
    U64, // u64
    F32, // f32
    F64, // f64
    Bool, // bool
    Hash, // #
    USize, // usize
    ISize, // isize
    DotComma, // ;
    Pack, // package;
    Import, // import
} TokenType;

typedef struct {
    TokenType type;
    void * data;
    uint64_t line;
    uint64_t index;
    char * path;
} Token;

typedef struct {
    int has_error;
    Token * tokens;
    uint64_t size;
    uint64_t capacity;
} Lexer;

void LexerNew(Lexer * self);
void LexerTokenise(Lexer * self, CompilationUnit * cu, Allocator * a);
void LexerPrint(Lexer * self);
void LexerPrintInitialisation(Lexer * self);

#endif