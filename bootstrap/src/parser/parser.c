#include "parser/parser.h"

#define PARSER_FIRST_PROGRAM (PARSER_FIRST_STATEMENTS)
#define PARSER_FIRST_STATEMENTS (PARSER_FIRST_STATEMENT)
#define PARSER_FIRST_STATEMENT (PARSER_FIRST_EXP)
#define PARSER_FIRST_EXP (PARSER_FIRST_NUMBER | PARSER_FIRST_WORD | PARSER_FIRST_OPS)
#define PARSER_FIRST_NUMBER (TOKEN_NUM)
#define PARSER_FIRST_WORD (TOKEN_WORD)
#define PARSER_FIRST_OPS (TOKEN_LPAREN | TOKEN_PLUS | TOKEN_MINUS)

static inline bool parser_pop_token(struct parser* parser, enum token_type type);
static inline bool parser_skip_except(struct parser* parser, enum token_type type);

static inline struct ast* parser_parse_program(struct parser* parser);
static inline struct ast* parser_parse_statements(struct parser* parser);
static inline struct ast* parser_parse_statement(struct parser* parser);
static inline struct ast* parser_parse_exp(struct parser* parser);
static inline struct ast* parser_parse_number(struct parser* parser);
static inline struct ast* parser_parse_word(struct parser* parser);
static inline struct ast* parser_parse_ops(struct parser* parser);

bool parser_of_file(struct parser* parser, char const* filename) { return lexer_of_file(&parser->lexer, filename); }

void parser_dtor(struct parser* parser) { lexer_dtor(&parser->lexer); }

struct ast* parser_parse(struct parser* parser) { return parser_parse_program(parser); }

static inline bool parser_peek_token(struct parser* parser, enum token_type type)
{
	struct token token = lexer_peek(&parser->lexer);
	if (token.type & type) return true;
	loc_print(&token.loc, stderr);
	fputs(": invalid token ", stderr);
	token_type_print(token.type, stderr);
	fputs(", expected ", stderr);
	token_types_print(type, stderr);
	fputc('\n', stderr);
	parser->error = true;
	return false;
}

static inline bool parser_pop_token(struct parser* parser, enum token_type type)
{
	if (!parser_peek_token(parser, type)) return false;
	struct token token = lexer_pop(&parser->lexer);
	token_dtor(&token);
	return true;
}

static inline bool parser_skip_except(struct parser* parser, enum token_type type)
{
	type |= TOKEN_EOF;
	if (lexer_peek(&parser->lexer).type & type) return false;
	struct token token = lexer_pop(&parser->lexer);
	token_dtor(&token);
	return true;
}

static inline struct ast* parser_parse_program(struct parser* parser)
{
	struct loc loc = lexer_peek(&parser->lexer).loc;
	struct ast* ast = ast_init(AST_SEQ, &loc);
	list_ctor(&ast->value.seq.list, &vlist_past, 16);
	while (lexer_peek(&parser->lexer).type & ~TOKEN_EOF)
	{
		struct ast* statements = parser_parse_statements(parser);
		if (statements->type == AST_ERROR)
			while (parser_skip_except(parser, TOKEN_NEWLINE)) {}
		seq_push(ast, statements);
		parser_pop_token(parser, TOKEN_NEWLINE | TOKEN_EOF);
	}
	if (ast->value.seq.list.size > 0) ast->loc = LOC(ast->loc, (*LIST_BACK(&ast->value.seq.list, struct ast*))->loc);
	parser_pop_token(parser, TOKEN_EOF);
	return ast;
}

static inline struct ast* parser_parse_statements(struct parser* parser)
{
	struct token token = lexer_peek(&parser->lexer);
	struct ast* ast = ast_init(AST_SEQ, &token.loc);
	list_ctor(&ast->value.seq.list, &vlist_past, 16);
	while (lexer_peek(&parser->lexer).type & ~(TOKEN_EOF | TOKEN_NEWLINE))
	{
		struct ast* statement = parser_parse_statement(parser);
		seq_push(ast, statement);
	}
	if (ast->value.seq.list.size > 0) ast->loc = LOC(ast->loc, (*LIST_BACK(&ast->value.seq.list, struct ast*))->loc);
	return ast;
}

static inline struct ast* parser_parse_statement(struct parser* parser)
{
	struct ast* exp = parser_parse_exp(parser);
	if (exp->type == AST_ERROR)
		while (parser_skip_except(parser, TOKEN_SEMICOLUMN | TOKEN_NEWLINE)) {}
	parser_pop_token(parser, TOKEN_SEMICOLUMN);
	return exp;
}

static inline struct ast* parser_parse_exp(struct parser* parser)
{
	DBG_ASSERT((PARSER_FIRST_NUMBER | PARSER_FIRST_WORD | PARSER_FIRST_OPS) == PARSER_FIRST_EXP
			   && "parser_parse_exp: missing case in parser");
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_STATEMENT)) return ast_init(AST_ERROR, &token.loc);
	if (token.type & PARSER_FIRST_NUMBER) return parser_parse_number(parser);
	if (token.type & PARSER_FIRST_WORD) return parser_parse_word(parser);
	if (token.type & PARSER_FIRST_OPS) return parser_parse_ops(parser);
	UNREACHABLE();
}

static inline struct ast* parser_parse_number(struct parser* parser)
{
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, TOKEN_NUM)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct ast* number = ast_init(AST_NUMBER, &token.loc);
	number->value.number.u64 = token.val.u64;
	token_dtor(&token);
	return number;
}

static inline struct ast* parser_parse_word(struct parser* parser)
{
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, TOKEN_WORD)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct ast* word = ast_init(AST_WORD, &token.loc);
	str_move(&word->value.word.str, &token.val.str);
	token_dtor(&token);
	return word;
}

static inline struct ast* parser_parse_ops(struct parser* parser)
{
	DBG_ASSERT((TOKEN_LPAREN | TOKEN_PLUS | TOKEN_MINUS) == PARSER_FIRST_OPS
			   && "parser_parse_ops: missing case in parser");
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_OPS)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct loc loc = token.loc;
	enum token_type type = token.type;
	token_dtor(&token);
	if (type == TOKEN_LPAREN)
	{
		struct ast* exp = parser_parse_exp(parser);
		// not sure if i should prioritize error detection for missing RPAREN and skip to RPAREN | NEWLINE
		// of this solution that skips to RPAREN in case of an error inside parens
		// TODO: find a way to cover both cases
		if (exp->type == AST_ERROR)
			while (parser_skip_except(parser, TOKEN_RPAREN)) {}
		parser_pop_token(parser, TOKEN_RPAREN);
		return exp;
	}
	if (type == TOKEN_MINUS || type == TOKEN_PLUS)
	{
		struct ast* rhs = parser_parse_exp(parser);
		loc = LOC(loc, rhs->loc);
		struct ast* unary = ast_init(AST_UNARY, &loc);
		unary->value.unary.op = type == TOKEN_MINUS ? UNARY_MINUS : UNARY_PLUS;
		unary->value.unary.rhs = rhs;
		return unary;
	}
	UNREACHABLE();
}
