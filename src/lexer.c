#include "lexer.h"
#include <stdlib.h>
#include "util.h"
#include <string.h>
#include "string_builder.h"
#include "ctype.h"
#include "string_slice.h"

#define DEBUG 0

void LexerNew(Lexer *self) {
    self->has_error = 0;
    self->tokens = 0;
    self->size = 0;
    self->capacity = 0;
}

void _LexerExpanding(Lexer * self, Allocator * a) {
    if(self->capacity == 0) {
        self->tokens = a->Alloc(sizeof(Token));
        if(self->tokens == NULL) terminate("Failed to initially allocate tokens for lexer\n");

        self->capacity = 1;
        return;
    }

    uint64_t new_capacity = self->capacity * 2;
    Token * old_tokens = self->tokens;

    self->tokens = a->Alloc(new_capacity * sizeof(Token));
    if(self->tokens == NULL) terminate("Failed to allocate tokkens for lexer\n");
    memcpy(self->tokens, old_tokens, self->size * sizeof(Token));
    a->Free(old_tokens);
    
    self->capacity = new_capacity;
}

int _LexerNeedExpanding(Lexer * self) {
    return self->size == self->capacity;
}

void _LexerAddToken(Lexer * self, TokenType type, void * data, uint64_t line, uint64_t index, char * path, Allocator * a) {
    if(_LexerNeedExpanding(self)) _LexerExpanding(self, a);
    Token * t = &self->tokens[self->size];
    t->type = type;
    t->data = data;
    t->line = line;
    t->index = index;
    t->path = path;

    self->size = self->size + 1;
}

void _LexerError() {}

// 13/09/2025 22:18
// This can be done wayyyyyy more efficient by just assigning a TokenType variable and writing the function call once
// But it's a Saturday evening and I am enjoying writing code plus I've come too far to redo this for now
// Deal with it later
// Or never

// TODO: make manual indexing safe by checking for nulll

void _LexerLexLine(Lexer * self, StringSlice * line_data, uint64_t line, CompilationUnit * cu, Allocator * a) {
    uint64_t index = 0;
    uint64_t advance = 0;
    TokenType type;
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
            case ',': type = Comma; advance = 1; break;
            case ':': type = Colon; advance = 1; break;
            case '(': type = LBra; advance = 1; break;
            case ')': type = RBra; advance = 1; break;
            case '{': type = LCur; advance = 1; break;
            case '}': type = RCur; advance = 1; break;
            case '[': type = LAng; advance = 1; break;
            case ']': type = RAng; advance = 1; break;
            case '+': type = Plus; advance = 1; break;
            case '*': type = Star; advance = 1; break;
            case '%': type = Perc; advance = 1; break;
            case '#': type = Hash; advance = 1; break;
            case '.': type = Dot; advance = 1; break;
            case ';': type = DotComma; advance = 1; break;
            case '/': {
                if(StringSliceAt(line_data, index + 1) == '/') {
                    // Comment, skip until the end of the line, so return
                    return;
                } else {
                    type = Slash;
                    advance = 1;
                }
            } break;

            // Double width tokens
            case '&': {
                if (StringSliceAt(line_data, index) == '&') {
                    type = AndAnd;
                    advance = 2;
                } else {
                    type = And;
                    advance = 1;
                }
            } break;
            case '|': {
                if (StringSliceAt(line_data, index) == '|') {
                    type = OrOr;
                    advance = 2;
                } else {
                    type = Or;
                    advance = 1;
                }
            } break;
            case '!': {
                if (StringSliceAt(line_data, index) == '=') {
                    type = BangEq;
                    advance = 2;
                } else {
                    type = Bang;
                    advance = 1;
                }
            } break;
            case '=': {
                if (StringSliceAt(line_data, index) == '=') {
                    type = EqEq;
                    advance = 2;
                } else {
                    type = Eq;
                    advance = 1;
                }
            } break;
            case '<': {
                if (StringSliceAt(line_data, index) == '=') {
                    type = LtEq;
                    advance = 2;
                } else if (StringSliceAt(line_data, index) == '<') {
                    type = LtLt;
                    advance = 2;
                }
                else {
                    type = Lt;
                    advance = 1;
                }
            } break;
            case '>': {
                if (StringSliceAt(line_data, index) == '=') {
                    type = GtEq;
                    advance = 2;
                } else if (StringSliceAt(line_data, index) == '>') {
                    type = GtGt;
                    advance = 2;
                }
                else {
                    type = Gt;
                    advance = 1;
                }
            } break;

            // '-', '->' or negative numeric literals
            case '-': {
                if(StringSliceAt(line_data, index) == '>') {
                    type = Arrow;
                    advance = 2;
                } else if(isdigit(StringSliceAt(line_data, index))) {
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

                    // TODO: handle error
                    if(has_dot > 1) {
                        _LexerError();
                    } else if(has_dot == 1) {
                        type = FloatLit;
                        data = StringBuilderBuild(&sb, a);
                    } else {
                        type = IntLit;
                        data = StringBuilderBuild(&sb, a);
                    }

                    advance = sb.size;
                    StringBuilderDelete(&sb, a);
                } else {
                    type = Min;
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
                
                // TODO: handle error
                if(has_dot > 1) {
                    _LexerError();
                } else if(has_dot == 1) {
                    type = FloatLit;
                    data = StringBuilderBuild(&sb, a);
                } else {
                    type = IntLit;
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

                type = StringLit;
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

                debug("Data %s", data);

                int is_id = 0;

                if(strcmp("if", data) == 0) { type = If; advance = 2; }
                else if(strcmp("else", data) == 0) { type = Else; advance = 4; }
                else if(strcmp("for", data) == 0) { type = For; advance = 3; }
                else if(strcmp("while", data) == 0) { type = While; advance = 5; }
                else if(strcmp("return", data) == 0) { type = Ret; advance = 6; }
                else if(strcmp("struct", data) == 0) { type = Struct; advance = 6; }
                else if(strcmp("fn", data) == 0) { type = Fn; advance = 2; }
                else if(strcmp("i8", data) == 0) { type = I8; advance = 2; }
                else if(strcmp("i16", data) == 0) { type = I16; advance = 3; }
                else if(strcmp("i32", data) == 0) { type = I32; advance = 3; }
                else if(strcmp("i64", data) == 0) { type = I64; advance = 3; }
                else if(strcmp("isize", data) == 0) { type = ISize; advance = 5; }
                else if(strcmp("u8", data) == 0) { type = U8; advance = 2; }
                else if(strcmp("u16", data) == 0) { type = U16; advance = 3; }
                else if(strcmp("u32", data) == 0) { type = U32; advance = 3; }
                else if(strcmp("u64", data) == 0) { type = U64; advance = 3; }
                else if(strcmp("usize", data) == 0) { type = USize; advance = 5; }
                else if(strcmp("f32", data) == 0) { type = F32; advance = 3; }
                else if(strcmp("f64", data) == 0) { type = F64; advance = 3; }
                else if(strcmp("bool", data) == 0) { type = Bool; advance = 4; }
                else if(strcmp("package", data) == 0) { type = Pack; advance = 7; }
                else if(strcmp("import", data) == 0) { type = Import; advance = 6; }
                else { type = Id; is_id = 1; advance = sb.size; }

                if(!is_id) { a->Free(data); data = 0; }
            } break;

            default: {
                printf("Unknown char: \"%c\", %d\n", c, c);
            }
        }

        _LexerAddToken(self, type, data, line, index, cu->path, a);

        index = index + advance;

        data = 0;
        advance = 0;
    }
}

void LexerTokenise(Lexer *self, CompilationUnit *cu, Allocator * a) {
    uint64_t line = 0;
    while(CompilationUnitHasLine(cu)) {
        StringSlice line_data = CompilationUnitGetLine(cu, a);
        _LexerLexLine(self, &line_data, line, cu, a);
        line = line + 1;
    }
}

char * _TokenTypeToString(TokenType type) {
    char * repr = 0;
    switch (type) {
        case Id: repr = "Id"; break;
        case IntLit: repr = "IntLit"; break;
        case FloatLit: repr = "FloatLit"; break;
        case StringLit: repr = "StringLit"; break;
        case Fn: repr = "Fn"; break;
        case Colon: repr = "Colon"; break;
        case Comma: repr = "Comma"; break;
        case LBra: repr = "LBra"; break;
        case RBra: repr = "RBra"; break;
        case LCur: repr = "LCur"; break;
        case RCur: repr = "RCur"; break;
        case LAng: repr = "LAng"; break;
        case RAng: repr = "RAng"; break;
        case Arrow: repr = "Arrow"; break;
        case Plus: repr = "Plus"; break;
        case Min: repr = "Min"; break;
        case Star: repr = "Star"; break;
        case Slash: repr = "Slash"; break;
        case Perc: repr = "Perc"; break;
        case Dot: repr = "Dot"; break;
        case And: repr = "And"; break;
        case Or: repr = "Or"; break;
        case AndAnd: repr = "AndAnd"; break;
        case OrOr: repr = "OrOr"; break;
        case LtLt: repr = "LtLt"; break;
        case GtGt: repr = "GtGt"; break;
        case Eq: repr = "Eq"; break;
        case EqEq: repr = "EqEq"; break;
        case Lt: repr = "Lt"; break;
        case Gt: repr = "Gt"; break;
        case LtEq: repr = "LtEq"; break;
        case GtEq: repr = "GtEq"; break;
        case Bang: repr = "Bang"; break;
        case BangEq: repr = "BangEq"; break;
        case If: repr = "If"; break;
        case Else: repr = "Else"; break;
        case While: repr = "While"; break;
        case For: repr = "For"; break;
        case Ret: repr = "Ret"; break;
        case Struct: repr = "Struct"; break;
        case I8: repr = "I8"; break;
        case I16: repr = "I16"; break;
        case I32: repr = "I32"; break;
        case I64: repr = "I64"; break;
        case U8: repr = "U8"; break;
        case U16: repr = "U16"; break;
        case U32: repr = "U32"; break;
        case U64: repr = "U64"; break;
        case F32: repr = "F32"; break;
        case F64: repr = "F64"; break;
        case Bool: repr = "Bool"; break;
        case Hash: repr = "Hash"; break;
        case USize: repr = "USize"; break;
        case ISize: repr = "ISize"; break;
        case DotComma: repr = "DotComma"; break;
        case Pack: repr = "Pack"; break;
        case Import: repr = "Import"; break;
        default: terminate("LexerPrint: unexpected token"); break;
    }
    return repr;
}

void LexerPrint(Lexer *self) {
    StringBuilder sb;
    StringBuilderNew(&sb);

    for(int i = 0; i < self->size; i++) {
        Token * token = &self->tokens[i];
        
        char * data = token->data;
        char * repr = _TokenTypeToString(token->type);
        if(data) {
            printf("[%s:%s %d:%d]", repr, data, token->line, token->index);
        } else {
            printf("[%s %d:%d]", repr, token->line, token->index);
        }
    }
}

void LexerPrintInitialisation(Lexer *self) {
    int i = 0;
    for(; i < self->size - 1; i++) {
        Token * token = &self->tokens[i];
        char * data = token->data;
        char * repr = _TokenTypeToString(token->type);

        if(data == 0) printf("{.type = %s, .data = 0, .line = %d, .index = %d, .path = \"%s\"}, ", repr, token->line, token->index, token->path);
        else printf("{.type = %s, .data = \"%s\", .line = %d, .index = %d, .path = \"%s\"}, ", repr, data, token->line, token->index, token->path);
    }

    Token * token = &self->tokens[i];
    char * data = token->data;
    char * repr = _TokenTypeToString(token->type);

    if(data == 0) printf("{.type = %s, .data = 0, .line = %d, .index = %d, .path = \"%s\"}\n", repr, token->line, token->index, token->path);
    else printf("{.type = %s, .data = \"%s\", .line = %d, .index = %d, .path = \"%s\"}\n", repr, data, token->line, token->index, token->path);
}
