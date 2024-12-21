#pragma once

// bootstrap
#include "lexer/token.h"
#include "stream/stream.h"

enum lexer_state
{
	LEXER_NONE = 0,
	LEXER_EOF,
};

struct lexer
{
	char const* filename;
	struct stream stream;
	struct token current;
	enum lexer_state state;
};

bool lexer_from_file(struct lexer* lexer, char const* filename);
void lexer_dtor(struct lexer* lexer);
struct token lexer_peek(struct lexer* lexer);
struct token lexer_pop(struct lexer* lexer);
