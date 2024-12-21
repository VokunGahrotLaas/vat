#pragma once

// libc
#include <stdio.h>
// bootstrap
#include "utils/str.h"

#define TOKEN_NONE ((enum token_type)0)

struct location
{
	char const* filename;
	size_t first_line;
	size_t first_column;
	size_t last_line;
	size_t last_column;
};

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
	struct location loc;
	enum token_type type;
	union token_value
	{
		struct str str;
		uint64_t u64;
	} val;
};

void location_print(struct location const* loc, FILE* stream);

bool token_of_type(struct token* token, struct location const* loc, enum token_type type);
bool token_ctor_word(struct token* token, struct location const* loc, struct str* str);
bool token_ctor_num(struct token* token, struct location const* loc, uint64_t u64);
void token_dtor(struct token* token);
void token_print(struct token* token, FILE* stream);
