#include "parser/parser.h"

static inline bool parser_assert_token(struct parser* parser, enum token_type type);
static inline bool parser_skip_inverse(struct parser* parser, enum token_type type);

static inline struct ast* parser_parse_program(struct parser* parser);
static inline struct ast* parser_parse_statements(struct parser* parser);

bool parser_of_file(struct parser* parser, char const* filename) { return lexer_of_file(&parser->lexer, filename); }

void parser_dtor(struct parser* parser) { lexer_dtor(&parser->lexer); }

struct ast* parser_parse(struct parser* parser) { return parser_parse_program(parser); }

static inline bool parser_assert_token(struct parser* parser, enum token_type type)
{
	struct token token = lexer_peek(&parser->lexer);
	if (token.type & type)
	{
		struct token token = lexer_pop(&parser->lexer);
		token_dtor(&token);
		return true;
	}
	loc_print(&token.loc, stderr);
	fprintf(stderr, ": invalid token (type 0x%" PRIx64 ")\n", (uint64_t)token.type);
	parser->error = true;
	return false;
}

static inline bool parser_skip_inverse(struct parser* parser, enum token_type type)
{
	type |= EOF;
	if ((lexer_peek(&parser->lexer).type & ~type) == 0) return false;
	struct token token = lexer_pop(&parser->lexer);
	token_dtor(&token);
	return true;
}

static inline struct ast* parser_parse_program(struct parser* parser)
{
	struct token token = lexer_peek(&parser->lexer);
	struct ast* ast = ast_init(AST_SEQ, &token.loc);
	while (lexer_peek(&parser->lexer).type != TOKEN_EOF)
	{
		struct ast* statements = parser_parse_statements(parser);
		if (!statements)
			while (parser_skip_inverse(parser, TOKEN_NEWLINE)) {}
		else if (parser_assert_token(parser, TOKEN_NEWLINE | TOKEN_EOF))
			seq_push(ast, statements);
	}
	parser_assert_token(parser, TOKEN_EOF);
	// ast->loc = LOC(ast->loc, token.loc);
	return ast;
}

static inline struct ast* parser_parse_statements(UNUSED struct parser* parser) { return NULL; }
