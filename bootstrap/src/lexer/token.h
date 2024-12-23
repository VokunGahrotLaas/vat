#pragma once

// libc
#include <stdio.h>
// bootstrap
#include "stream/location.h"
#include "utils/str.h"

enum token_type
{
#define TOKEN_NONE ((enum token_type)0)
#define TOKEN_MIN TOKEN_ERROR
	// value: none
	TOKEN_ERROR = 1 << 0,
	TOKEN_NEWLINE = 1 << 1,
	TOKEN_EOF = 1 << 2,
	/// punct
	TOKEN_SEMICOLON = 1 << 3,
	TOKEN_LPAREN = 1 << 4,
	TOKEN_RPAREN = 1 << 5,
	TOKEN_PLUS = 1 << 6,
	TOKEN_MINUS = 1 << 7,
	TOKEN_EQUAL = 1 << 8,
	TOKEN_COLON = 1 << 9,
	TOKEN_COMA = 1 << 10,
	// words
	TOKEN_LET = 1 << 11,
	// value: str
	TOKEN_WORD = 1 << 12,
	TOKEN_STRLIT = 1 << 13,
	// value: u64
	TOKEN_NUMLIT = 1 << 14,
#define TOKEN_MAX (TOKEN_NUMLIT + 1)
};

struct token
{
	struct loc loc;
	enum token_type type;
	union token_value
	{
		struct str str;
		uint64_t u64;
	} val;
};

bool token_of_type(struct token* token, struct loc const* loc, enum token_type type);
bool token_ctor_word(struct token* token, struct loc const* loc, struct str* str);
bool token_ctor_strlit(struct token* token, struct loc const* loc, struct str* str);
bool token_ctor_numlit(struct token* token, struct loc const* loc, uint64_t u64);
void token_dtor(struct token* token);
void token_print(struct token* token, FILE* stream);
void token_type_print(enum token_type type, FILE* stream);
void token_types_print(enum token_type type, FILE* stream);
