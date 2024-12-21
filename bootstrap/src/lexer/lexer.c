#include "lexer/lexer.h"

// libc
#include <ctype.h>

static inline struct location lexer_loc_ctor(struct lexer* lexer);
static inline void lexer_loc_end(struct lexer* lexer, struct location* loc);

static inline void lexer_lex(struct lexer* lexer);
static inline void lexer_lex_u64(struct lexer* lexer);
static inline void lexer_lex_str(struct lexer* lexer);

bool lexer_from_file(struct lexer* lexer, char const* filename)
{
	lexer->filename = filename;
	lexer->state = LEXER_NONE;
	token_from_type(&lexer->current, NULL, TOKEN_NONE);
	return stream_from_file(&lexer->stream, filename);
}

void lexer_dtor(struct lexer* lexer)
{
	token_dtor(&lexer->current);
	stream_dtor(&lexer->stream);
}

struct token lexer_peek(struct lexer* lexer)
{
	if (lexer->current.type != TOKEN_NONE) return lexer->current;
	switch (lexer->state)
	{
	case LEXER_NONE: lexer_lex(lexer); break;
	case LEXER_EOF: {
		struct location loc = lexer_loc_ctor(lexer);
		token_from_type(&lexer->current, &loc, TOKEN_EOF);
		break;
	}
	};
	return lexer->current;
}

struct token lexer_pop(struct lexer* lexer)
{
	struct token current = lexer_peek(lexer);
	token_from_type(&lexer->current, NULL, TOKEN_NONE);
	return current;
}

static inline struct location lexer_loc_ctor(struct lexer* lexer)
{
	return (struct location){
		.filename = lexer->filename,
		.first_line = lexer->stream.line,
		.first_column = lexer->stream.column,
		.last_line = lexer->stream.line,
		.last_column = lexer->stream.column,
	};
}

static inline void lexer_loc_end(struct lexer* lexer, struct location* loc)
{
	loc->last_line = lexer->stream.line;
	loc->last_column = lexer->stream.column;
}

static inline void lexer_lex(struct lexer* lexer)
{
	while (isspace(stream_peek(&lexer->stream)) && stream_peek(&lexer->stream) != '\n')
		stream_pop(&lexer->stream);
	if (stream_peek(&lexer->stream) == '\n')
	{
		stream_pop(&lexer->stream);
		struct location loc = lexer_loc_ctor(lexer);
		lexer_loc_end(lexer, &loc);
		token_from_type(&lexer->current, &loc, TOKEN_NEWLINE);
	}
	else if (stream_peek(&lexer->stream) == EOF)
	{
		struct location loc = lexer_loc_ctor(lexer);
		lexer_loc_end(lexer, &loc);
		token_from_type(&lexer->current, &loc, TOKEN_EOF);
		lexer->state = LEXER_EOF;
	}
	else if (isdigit(stream_peek(&lexer->stream)))
		lexer_lex_u64(lexer);
	else
		lexer_lex_str(lexer);
}

static inline void lexer_lex_u64(struct lexer* lexer)
{
	struct location loc = lexer_loc_ctor(lexer);
	uint64_t v = 0;
	while (isdigit(stream_peek(&lexer->stream)))
	{
		char c = stream_pop(&lexer->stream);
		v *= 10;
		v += c - '0';
	}
	lexer_loc_end(lexer, &loc);
	token_ctor_u64(&lexer->current, &loc, v);
}

static inline void lexer_lex_str(struct lexer* lexer)
{
	struct location loc = lexer_loc_ctor(lexer);
	struct str v;
	str_ctor(&v, 16);
	while (!isspace(stream_peek(&lexer->stream)))
	{
		char c = stream_pop(&lexer->stream);
		str_pushc(&v, c);
	}
	lexer_loc_end(lexer, &loc);
	token_ctor_str(&lexer->current, &loc, &v);
}
