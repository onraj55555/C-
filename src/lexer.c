#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>
#include "string_builder.h"
#include "ctype.h"
#include "string_slice.h"
#include <stdarg.h>

#include "lexer.h"

#define DEBUG 0

/*
 * Initialise a new lexer
 * @param self A pointer to a Lexer object
*/
void LexerNew(Lexer *self) {
    self->has_error = 0;
    DA_INIT(&self->tokens);
    //da_Token_new(&self->tokens);
}

/*
 * Private function, adds a token to the internal dynamic array of tokens
 * @param self A pointer to a Lexer object
 * @param type The type of token that this is about
 * @param data The owned data of the token
 * @param line The line this tokens can be found on
 * @param index The index of the token on the line
 * @param path The file in which the token can be found
 * @param a A pointer to an Allocator object
 */
void _LexerAddToken(Lexer * self, TokenType type, void * data, uint64_t line, uint64_t index, char * path, allocator_t * a) {
    Token t = { 0 };
    t.type = type;
    t.data = data;
    t.line = line;
    t.index = index;
    t.path = path;
    DA_PUSHBACK(&self->tokens, t, a);
    //da_Token_pushback(&self->tokens, &t, a);
}

void _LexerError(Lexer * self, const char * path, uint64_t line, uint64_t index, const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Lexer error at %s[%ld:%ld]: ", path, line, index);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

}

// 13/09/2025 22:18
// This can be done wayyyyyy more efficient by just assigning a TokenType variable and writing the function call once
// But it's a Saturday evening and I am enjoying writing code plus I've come too far to redo this for now
// Deal with it later
// Or never

// TODO: make manual indexing safe by checking for nulll

/*
 * Private function, lexes 1 full line of the source file
 * @param self A pointer to a Lexer object
 * @param line_data A pointer to a StringSlice object that describes an entire line
 * @param line The line this is about
 * @param cu A pointer to the CompilationUnit this line belongs to
 * @param a A pointer to an Allocator object
 */
void _LexerLexLine(Lexer * self, StringSlice * line_data, uint64_t line, CompilationUnit * cu, allocator_t * a) {
    uint64_t index = 0;
    uint64_t advance = 0;
    TokenType type;
    int error = 0;
    void * data = NULL;
    char * path = cu->path;

    while(index < line_data->len) {
        char c = StringSliceAt(line_data, index);
        debug("Lexer: c = %c, %d\n", c, c);

        switch(c) {
            // White spaces
            case ' ':
            case '\t': index = index + 1; continue; // TODO this is a bug

            // Single width tokens
            case ',': type = L_Comma; advance = 1; break;
            case ':': type = L_Colon; advance = 1; break;
            case '(': type = L_LBra; advance = 1; break;
            case ')': type = L_RBra; advance = 1; break;
            case '{': type = L_LCur; advance = 1; break;
            case '}': type = L_RCur; advance = 1; break;
            case '[': type = L_LAng; advance = 1; break;
            case ']': type = L_RAng; advance = 1; break;
            case '+': type = L_Plus; advance = 1; break;
            case '*': type = L_Star; advance = 1; break;
            case '%': type = L_Perc; advance = 1; break;
            case '#': type = L_Hash; advance = 1; break;
            case '.': type = L_Dot; advance = 1; break;
            case ';': type = L_DotComma; advance = 1; break;
            case '/': {
                if(StringSliceAt(line_data, index + 1) == '/') {
                    // Comment, skip until the end of the line, so return
                    return;
                } else {
                    type = L_Slash;
                    advance = 1;
                }
            } break;

            // Double width tokens
            case '&': {
                if (StringSliceAt(line_data, index + 1) == '&') {
                    type = L_AndAnd;
                    advance = 2;
                } else {
                    type = L_And;
                    advance = 1;
                }
            } break;
            case '|': {
                if (StringSliceAt(line_data, index + 1) == '|') {
                    type = L_OrOr;
                    advance = 2;
                } else {
                    type = L_Or;
                    advance = 1;
                }
            } break;
            case '!': {
                if (StringSliceAt(line_data, index + 1) == '=') {
                    type = L_BangEq;
                    advance = 2;
                } else {
                    type = L_Bang;
                    advance = 1;
                }
            } break;
            case '=': {
                if (StringSliceAt(line_data, index + 1) == '=') {
                    type = L_EqEq;
                    advance = 2;
                } else {
                    type = L_Eq;
                    advance = 1;
                }
            } break;
            case '<': {
                if (StringSliceAt(line_data, index + 1) == '=') {
                    type = L_LtEq;
                    advance = 2;
                } else if (StringSliceAt(line_data, index + 1) == '<') {
                    type = L_LtLt;
                    advance = 2;
                }
                else {
                    type = L_Lt;
                    advance = 1;
                }
            } break;
            case '>': {
                if (StringSliceAt(line_data, index + 1) == '=') {
                    type = L_GtEq;
                    advance = 2;
                } else if (StringSliceAt(line_data, index + 1) == '>') {
                    type = L_GtGt;
                    advance = 2;
                }
                else {
                    type = L_Gt;
                    advance = 1;
                }
            } break;

            // '-', '->' or negative numeric literals
            case '-': {
                if(StringSliceAt(line_data, index + 1) == '>') {
                    type = L_Arrow;
                    advance = 2;
                } else if(isdigit(StringSliceAt(line_data, index + 1))) {
                    // Note: advance does not need to be kept because sb.size will be used for that
                    StringBuilder sb;
                    StringBuilderNew(&sb);
                    StringBuilderPushChar(&sb, c, a); // Add '-'

                    int has_dot = 0;
                    c = StringSliceAt(line_data, index + sb.size);
                    while(c >= '0' && c <= '9' || c == '.') {
                        if(c == '.') has_dot++;
                        StringBuilderPushChar(&sb, c, a);
                        c = StringSliceAt(line_data, index + sb.size);
                    }

                    if(has_dot > 1) {
                        _LexerError(self, path, line, index, "too many decimal places in float literal, counted %d", has_dot);
                        self->has_error = 1;
                        error = 1;
                    } else if(has_dot == 1) {
                        type = L_FloatLit;
                        data = StringBuilderBuild(&sb, a);
                    } else {
                        type = L_IntLit;
                        data = StringBuilderBuild(&sb, a);
                    }

                    advance = sb.size;
                    StringBuilderDelete(&sb, a);
                } else {
                    type = L_Min;
                    advance = 1;
                } break;
            }

            // Number literals
            case '0' ... '9': {
                StringBuilder sb;
                StringBuilderNew(&sb);
                StringBuilderPushChar(&sb, c, a); // Add '-'

                int has_dot = 0;
                c = StringSliceAt(line_data, index + sb.size);
                while(c >= '0' && c <= '9' || c == '.') {
                    if(c == '.') has_dot++;
                    StringBuilderPushChar(&sb, c, a);
                    c = StringSliceAt(line_data, index + sb.size);
                }

                if(has_dot > 1) {
                    _LexerError(self, path, line, index, "too many decimal places in float literal, counted %d", has_dot);
                    self->has_error = 1;
                    error = 1;
                } else if(has_dot == 1) {
                    type = L_FloatLit;
                    data = StringBuilderBuild(&sb, a);
                } else {
                    type = L_IntLit;
                    data = StringBuilderBuild(&sb, a);
                }

                advance = sb.size;
                StringBuilderDelete(&sb, a);
            } break;

            // String literals
            case '"': {
                StringBuilder sb;
                StringBuilderNew(&sb);

                c = StringSliceAt(line_data, index + sb.size + 1);
                while(c != '"') {
                    StringBuilderPushChar(&sb, c, a);
                    c = StringSliceAt(line_data, index + sb.size + 1);
                }

                type = L_StringLit;
                data = StringBuilderBuild(&sb, a);
                advance = sb.size + 2; // Ommit the starting and ending "
                StringBuilderDelete(&sb, a);
            } break;

            // Identifiers (including keywords)
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_': {
                StringBuilder sb;
                StringBuilderNew(&sb);
                StringBuilderPushChar(&sb, c, a);

                c = StringSliceAt(line_data, index + sb.size);
                while(isalnum(c) || c == '_') {
                    StringBuilderPushChar(&sb, c, a);
                    c = StringSliceAt(line_data, index + sb.size);
                }

                advance = sb.size;

                data = StringBuilderBuild(&sb, a);

                debug("Data %s", (char *)data);

                int is_id = 0;

                if(strcmp("if", data) == 0) { type = L_If; advance = 2; }
                else if(strcmp("else", data) == 0) { type = L_Else; advance = 4; }
                else if(strcmp("for", data) == 0) { type = L_For; advance = 3; }
                else if(strcmp("while", data) == 0) { type = L_While; advance = 5; }
                else if(strcmp("return", data) == 0) { type = L_Ret; advance = 6; }
                else if(strcmp("struct", data) == 0) { type = L_Struct; advance = 6; }
                else if(strcmp("fn", data) == 0) { type = L_Fn; advance = 2; }
                else if(strcmp("i8", data) == 0) { type = L_I8; advance = 2; }
                else if(strcmp("i16", data) == 0) { type = L_I16; advance = 3; }
                else if(strcmp("i32", data) == 0) { type = L_I32; advance = 3; }
                else if(strcmp("i64", data) == 0) { type = L_I64; advance = 3; }
                else if(strcmp("isize", data) == 0) { type = L_ISize; advance = 5; }
                else if(strcmp("u8", data) == 0) { type = L_U8; advance = 2; }
                else if(strcmp("u16", data) == 0) { type = L_U16; advance = 3; }
                else if(strcmp("u32", data) == 0) { type = L_U32; advance = 3; }
                else if(strcmp("u64", data) == 0) { type = L_U64; advance = 3; }
                else if(strcmp("usize", data) == 0) { type = L_USize; advance = 5; }
                else if(strcmp("f32", data) == 0) { type = L_F32; advance = 3; }
                else if(strcmp("f64", data) == 0) { type = L_F64; advance = 3; }
                else if(strcmp("bool", data) == 0) { type = L_Bool; advance = 4; }
                else if(strcmp("mod", data) == 0) { type = L_Mod; advance = 7; }
                else if(strcmp("use", data) == 0) { type = L_Use; advance = 6; }
                else if(strcmp("void", data) == 0) { type = L_Void; advance = 4; }
                else if(strcmp("let", data) == 0) { type = L_Let; advance = 3; }
                else { type = L_Id; is_id = 1; advance = sb.size; }

                if(!is_id) { allocator_free(a, data); data = 0; }
            } break;

            default: {
                // TODO: handle error
                _LexerError(self, path, line, index, "unknown char '%c' (%d)", c, c);
                self->has_error = 1;
                error = 1;
            }
        }

        if(!error) _LexerAddToken(self, type, data, line, index, cu->path, a);
        error = 0;

        index = index + advance;

        data = 0;
        advance = 0;
    }
}

/*
 * Lex a full compilation unit and store the output in the internal dynamic array
 * @param self A pointer to a Lexer object
 * @param cu A pointer to a CompilationUnit object
 * @param a A pointer to an Allocator object
 */
void LexerTokenise(Lexer *self, CompilationUnit *cu, allocator_t * a) {
    uint64_t line = 0;
    while(CompilationUnitHasLine(cu)) {
        StringSlice line_data = CompilationUnitGetLine(cu, a);
        _LexerLexLine(self, &line_data, line, cu, a);
        line = line + 1;
    }
    Token eof = { 0 };
    eof.type = L_EOF;
    eof.data = 0;
    eof.path = cu->path;
    eof.line = 0;
    eof.index = 0;
    DA_PUSHBACK(&self->tokens, eof, a);
    //da_Token_pushback(&self->tokens, &eof, a);
}

char * _TokenTypeToString(TokenType type) {
    char * repr = 0;
    switch (type) {
        case L_Id: repr = "Id"; break;
        case L_IntLit: repr = "IntLit"; break;
        case L_FloatLit: repr = "FloatLit"; break;
        case L_StringLit: repr = "StringLit"; break;
        case L_Fn: repr = "Fn"; break;
        case L_Colon: repr = "Colon"; break;
        case L_Comma: repr = "Comma"; break;
        case L_LBra: repr = "LBra"; break;
        case L_RBra: repr = "RBra"; break;
        case L_LCur: repr = "LCur"; break;
        case L_RCur: repr = "RCur"; break;
        case L_LAng: repr = "LAng"; break;
        case L_RAng: repr = "RAng"; break;
        case L_Arrow: repr = "Arrow"; break;
        case L_Plus: repr = "Plus"; break;
        case L_Min: repr = "Min"; break;
        case L_Star: repr = "Star"; break;
        case L_Slash: repr = "Slash"; break;
        case L_Perc: repr = "Perc"; break;
        case L_Dot: repr = "Dot"; break;
        case L_And: repr = "And"; break;
        case L_Or: repr = "Or"; break;
        case L_AndAnd: repr = "AndAnd"; break;
        case L_OrOr: repr = "OrOr"; break;
        case L_LtLt: repr = "LtLt"; break;
        case L_GtGt: repr = "GtGt"; break;
        case L_Eq: repr = "Eq"; break;
        case L_EqEq: repr = "EqEq"; break;
        case L_Lt: repr = "Lt"; break;
        case L_Gt: repr = "Gt"; break;
        case L_LtEq: repr = "LtEq"; break;
        case L_GtEq: repr = "GtEq"; break;
        case L_Bang: repr = "Bang"; break;
        case L_BangEq: repr = "BangEq"; break;
        case L_If: repr = "If"; break;
        case L_Else: repr = "Else"; break;
        case L_While: repr = "While"; break;
        case L_For: repr = "For"; break;
        case L_Ret: repr = "Ret"; break;
        case L_Struct: repr = "Struct"; break;
        case L_I8: repr = "I8"; break;
        case L_I16: repr = "I16"; break;
        case L_I32: repr = "I32"; break;
        case L_I64: repr = "I64"; break;
        case L_U8: repr = "U8"; break;
        case L_U16: repr = "U16"; break;
        case L_U32: repr = "U32"; break;
        case L_U64: repr = "U64"; break;
        case L_F32: repr = "F32"; break;
        case L_F64: repr = "F64"; break;
        case L_Bool: repr = "Bool"; break;
        case L_Hash: repr = "Hash"; break;
        case L_USize: repr = "USize"; break;
        case L_ISize: repr = "ISize"; break;
        case L_DotComma: repr = "DotComma"; break;
        case L_Use: repr = "Use"; break;
        case L_Mod: repr = "Mod"; break;
        case L_EOF: repr = "EOF"; break;
        case L_Let: repr = "Let"; break;
        default: terminate("LexerPrint: unexpected token"); break;
    }
    return repr;
}

void LexerPrint(Lexer *self) {
    StringBuilder sb;
    StringBuilderNew(&sb);

    for(int i = 0; i < self->tokens.size; i++) {
        Token * token = &self->tokens.data[i];

        char * data = token->data;
        char * repr = _TokenTypeToString(token->type);
        if(data) {
            printf("[%s:%s %lu:%lu]", repr, data, token->line, token->index);
        } else {
            printf("[%s %lu:%lu]", repr, token->line, token->index);
        }
    }
}

void LexerPrintInitialisation(Lexer *self) {
    int i = 0;
    for(; i < self->tokens.size - 1; i++) {
        Token * token = &self->tokens.data[i];
        char * data = token->data;
        char * repr = _TokenTypeToString(token->type);

        if(data == 0) printf("{.type = %s, .data = 0, .line = %lu, .index = %lu, .path = \"%s\"}, ", repr, token->line, token->index, token->path);
        else printf("{.type = %s, .data = \"%s\", .line = %lu, .index = %lu, .path = \"%s\"}, ", repr, data, token->line, token->index, token->path);
    }

    Token * token = &self->tokens.data[i];
    char * data = token->data;
    char * repr = _TokenTypeToString(token->type);

    if(data == 0) printf("{.type = %s, .data = 0, .line = %lu, .index = %lu, .path = \"%s\"}\n", repr, token->line, token->index, token->path);
    else printf("{.type = %s, .data = \"%s\", .line = %lu, .index = %lu, .path = \"%s\"}\n", repr, data, token->line, token->index, token->path);
}
