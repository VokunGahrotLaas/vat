#pragma once

// libc
#include <stdio.h>
// bootstrap
#include "stream/location.h"
#include "utils/str.h"

#define TOKEN_NONE ((enum token_type)0)

enum token_type
{
	// value: none
	TOKEN_NEWLINE = 1,
	/// punct
	TOKEN_SEMICOLUMN,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_PLUS,
	TOKEN_MINUS,
	/// other
	TOKEN_EOF,
	// value: str
	TOKEN_WORD,
	// value: u64
	TOKEN_NUM,
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
bool token_ctor_num(struct token* token, struct loc const* loc, uint64_t u64);
void token_dtor(struct token* token);
void token_print(struct token* token, FILE* stream);
