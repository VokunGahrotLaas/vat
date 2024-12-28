#include "lexer/lexer.h"

// libc
#include <ctype.h>

static inline struct loc lexer_loc_ctor(struct lexer* lexer);
static inline struct loc lexer_loc_ctor_peek(struct lexer* lexer);
static inline void lexer_loc_end(struct lexer* lexer, struct loc* loc);

static inline bool is_letter(int c);
static inline bool is_op(int c);

static inline void lexer_lex(struct lexer* lexer);
static inline void lexer_lex_numlit(struct lexer* lexer);
static inline void lexer_lex_word(struct lexer* lexer);
static inline void lexer_lex_op(struct lexer* lexer);
static inline void lexer_lex_strlit(struct lexer* lexer);

bool lexer_of_file(struct lexer* lexer, char const* filename)
{
	lexer->error = false;
	lexer->state = LEXER_NONE;
	token_of_type(&lexer->current, NULL, TOKEN_NONE);
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
		struct loc loc = lexer_loc_ctor(lexer);
		token_of_type(&lexer->current, &loc, TOKEN_EOF);
		break;
	}
	};
	return lexer->current;
}

struct token lexer_pop(struct lexer* lexer)
{
	struct token current = lexer_peek(lexer);
	token_of_type(&lexer->current, NULL, TOKEN_NONE);
	return current;
}

static inline struct loc lexer_loc_ctor(struct lexer* lexer)
{
	return (struct loc){
		.filename = lexer->stream.filename,
		.first_line = lexer->stream.pos.line,
		.first_column = lexer->stream.pos.column,
		.last_line = lexer->stream.pos.line,
		.last_column = lexer->stream.pos.column,
	};
}

static inline struct loc lexer_loc_ctor_peek(struct lexer* lexer)
{
	return (struct loc){
		.filename = lexer->stream.filename,
		.first_line = lexer->stream.peek_pos.line,
		.first_column = lexer->stream.peek_pos.column,
		.last_line = lexer->stream.peek_pos.line,
		.last_column = lexer->stream.peek_pos.column,
	};
}

static inline void lexer_loc_end(struct lexer* lexer, struct loc* loc)
{
	loc->last_line = lexer->stream.pos.line;
	loc->last_column = lexer->stream.pos.column;
}

static inline bool is_letter(int c) { return isalnum(c) || c == '_'; }

static inline bool is_op(int c)
{
	switch (c)
	{
	case ';': FALLTHROUGH;
	case '(': FALLTHROUGH;
	case ')': FALLTHROUGH;
	case '+': FALLTHROUGH;
	case '-': FALLTHROUGH;
	case '=': FALLTHROUGH;
	case ':': FALLTHROUGH;
	case ',': FALLTHROUGH;
	case '{': FALLTHROUGH;
	case '}': FALLTHROUGH;
	case '.': FALLTHROUGH;
	case '@': return true;
	default: return false;
	};
}

static inline void lexer_lex(struct lexer* lexer)
{
	if (stream_peek(&lexer->stream) == '\n')
	{
		stream_pop(&lexer->stream);
		struct loc loc = lexer_loc_ctor(lexer);
		token_of_type(&lexer->current, &loc, TOKEN_NEWLINE);
	}
	else if (isspace(stream_peek(&lexer->stream)))
	{
		stream_pop(&lexer->stream);
		struct loc loc = lexer_loc_ctor(lexer);
		while (isspace(stream_peek(&lexer->stream)) && stream_peek(&lexer->stream) != '\n')
			stream_pop(&lexer->stream);
		lexer_loc_end(lexer, &loc);
		token_of_type(&lexer->current, &loc, TOKEN_WHITESPACE);
	}
	else if (stream_peek(&lexer->stream) == EOF)
	{
		stream_pop(&lexer->stream);
		struct loc loc = lexer_loc_ctor(lexer);
		token_of_type(&lexer->current, &loc, TOKEN_EOF);
		lexer->state = LEXER_EOF;
	}
	else if (isdigit(stream_peek(&lexer->stream)))
		lexer_lex_numlit(lexer);
	else if (is_letter(stream_peek(&lexer->stream)))
		lexer_lex_word(lexer);
	else if (is_op(stream_peek(&lexer->stream)))
		lexer_lex_op(lexer);
	else if (stream_peek(&lexer->stream) == '"')
		lexer_lex_strlit(lexer);
	else
	{
		struct loc loc = lexer_loc_ctor_peek(lexer);
		lexer->error = true;
		loc_print(&loc, stderr);
		fprintf(stderr, ": invalid character found (%c)\n", (unsigned char)stream_peek(&lexer->stream));
		stream_pop(&lexer->stream);
		token_of_type(&lexer->current, &loc, TOKEN_ERROR);
	}
}

static inline void lexer_lex_numlit(struct lexer* lexer)
{
	struct loc loc = lexer_loc_ctor_peek(lexer);
	uint64_t v = 0;
	while (isdigit(stream_peek(&lexer->stream)))
	{
		char c = stream_pop(&lexer->stream);
		v *= 10;
		v += c - '0';
	}
	lexer_loc_end(lexer, &loc);
	token_ctor_numlit(&lexer->current, &loc, v);
}

static inline void lexer_lex_word(struct lexer* lexer)
{
	struct loc loc = lexer_loc_ctor_peek(lexer);
	struct str v;
	str_ctor(&v, 16);
	while (is_letter(stream_peek(&lexer->stream)))
	{
		char c = stream_pop(&lexer->stream);
		str_pushc(&v, c);
	}
	lexer_loc_end(lexer, &loc);
	if (cv_cmp(cv_str(&v), cv_cstr("let")) == 0)
		token_of_type(&lexer->current, &loc, TOKEN_LET);
	else if (cv_cmp(cv_str(&v), cv_cstr("fn")) == 0)
		token_of_type(&lexer->current, &loc, TOKEN_FN);
	else if (cv_cmp(cv_str(&v), cv_cstr("ret")) == 0)
		token_of_type(&lexer->current, &loc, TOKEN_RET);
	else if (cv_cmp(cv_str(&v), cv_cstr("module")) == 0)
		token_of_type(&lexer->current, &loc, TOKEN_MODULE);
	else if (cv_cmp(cv_str(&v), cv_cstr("import")) == 0)
		token_of_type(&lexer->current, &loc, TOKEN_IMPORT);
	else if (cv_cmp(cv_str(&v), cv_cstr("as")) == 0)
		token_of_type(&lexer->current, &loc, TOKEN_AS);
	else
	{
		token_ctor_word(&lexer->current, &loc, &v);
		return;
	}
	str_dtor(&v);
}

static inline void lexer_lex_op(struct lexer* lexer)
{
	struct loc loc = lexer_loc_ctor_peek(lexer);
	int c = stream_pop(&lexer->stream);
	if (c == ';')
		token_of_type(&lexer->current, &loc, TOKEN_SEMICOLON);
	else if (c == '(')
		token_of_type(&lexer->current, &loc, TOKEN_LPAREN);
	else if (c == ')')
		token_of_type(&lexer->current, &loc, TOKEN_RPAREN);
	else if (c == '+')
		token_of_type(&lexer->current, &loc, TOKEN_PLUS);
	else if (c == '-')
	{
		int c = stream_peek(&lexer->stream);
		if (c == '>')
		{
			stream_pop(&lexer->stream);
			lexer_loc_end(lexer, &loc);
			token_of_type(&lexer->current, &loc, TOKEN_ARROW);
		}
		else
			token_of_type(&lexer->current, &loc, TOKEN_MINUS);
	}
	else if (c == '=')
		token_of_type(&lexer->current, &loc, TOKEN_EQUAL);
	else if (c == ':')
		token_of_type(&lexer->current, &loc, TOKEN_COLON);
	else if (c == ',')
		token_of_type(&lexer->current, &loc, TOKEN_COMA);
	else if (c == '{')
		token_of_type(&lexer->current, &loc, TOKEN_LCURLBRA);
	else if (c == '}')
		token_of_type(&lexer->current, &loc, TOKEN_RCURLBRA);
	else if (c == '.')
		token_of_type(&lexer->current, &loc, TOKEN_DOT);
	else if (c == '@')
		token_of_type(&lexer->current, &loc, TOKEN_AT);
	else
	{
		lexer->error = true;
		loc_print(&loc, stderr);
		fprintf(stderr, ": invalid operator found (%c)\n", c);
		token_of_type(&lexer->current, &loc, TOKEN_ERROR);
	}
}

static inline void lexer_lex_strlit(struct lexer* lexer)
{
	struct loc loc = lexer_loc_ctor_peek(lexer);
	if (stream_peek(&lexer->stream) != '"')
	{
		token_of_type(&lexer->current, &loc, TOKEN_ERROR);
		return;
	}
	stream_pop(&lexer->stream);
	struct str v;
	str_ctor(&v, 16);
	bool escaped = false;
	while (escaped || stream_peek(&lexer->stream) != '"')
	{
		char c = stream_pop(&lexer->stream);
		if (escaped)
		{
			if (c == '\\' || c == '"')
				str_pushc(&v, c);
			else if (c == 'n')
				str_pushc(&v, '\n');
			else if (c == 't')
				str_pushc(&v, '\t');
			else
			{
				lexer->error = true;
				loc_print(&loc, stderr);
				fprintf(stderr, ": invalid escaped character in string litteral (0x%02x)\n", (uint8_t)c);
			}
			escaped = false;
		}
		else
		{
			if (c == '\\')
				escaped = true;
			else
				str_pushc(&v, c);
		}
	}
	if (stream_peek(&lexer->stream) != '"')
	{
		token_of_type(&lexer->current, &loc, TOKEN_ERROR);
		return;
	}
	stream_pop(&lexer->stream);
	lexer_loc_end(lexer, &loc);
	token_ctor_strlit(&lexer->current, &loc, &v);
}
