#include "parser/parser.h"

#define PARSER_FIRST_PROGRAM (PARSER_FIRST_STATEMENTS)
#define PARSER_FIRST_STATEMENTS (PARSER_FIRST_STATEMENT)
#define PARSER_FIRST_STATEMENT (PARSER_FIRST_EXP | PARSER_FIRST_ASSIGN)
#define PARSER_FIRST_EXP                                                                                               \
	(PARSER_FIRST_NUMLIT | PARSER_FIRST_STRLIT | PARSER_FIRST_VAR | PARSER_FIRST_OPS | PARSER_FIRST_CALL)
#define PARSER_FIRST_NUMLIT (TOKEN_NUMLIT)
#define PARSER_FIRST_STRLIT (TOKEN_STRLIT)
#define PARSER_FIRST_VAR (TOKEN_WORD)
#define PARSER_FIRST_OPS (TOKEN_LPAREN | TOKEN_PLUS | TOKEN_MINUS)
#define PARSER_FIRST_LEXP (PARSER_FIRST_VAR)
#define PARSER_FIRST_TEXP (PARSER_FIRST_VAR)
#define PARSER_FIRST_ASSIGN (TOKEN_LET)
#define PARSER_FIRST_CALL (PARSER_FIRST_LEXP)

static inline enum token_type parser_peek_token(struct parser* parser, enum token_type type);
static inline enum token_type parser_pop_token(struct parser* parser, enum token_type type);
static inline bool parser_skip_except(struct parser* parser, enum token_type type);

static inline struct ast* parser_parse_program(struct parser* parser);
static inline struct ast* parser_parse_statements(struct parser* parser);
static inline struct ast* parser_parse_statement(struct parser* parser);
static inline struct ast* parser_parse_exp(struct parser* parser);
static inline struct ast* parser_parse_numlit(struct parser* parser);
static inline struct ast* parser_parse_strlit(struct parser* parser);
static inline struct ast* parser_parse_var(struct parser* parser);
static inline struct ast* parser_parse_ops(struct parser* parser);
static inline struct ast* parser_parse_lexp(struct parser* parser);
static inline struct ast* parser_parse_texp(struct parser* parser);
static inline struct ast* parser_parse_assign(struct parser* parser);
static inline struct ast* parser_parse_call(struct parser* parser, struct ast* lexp);

bool parser_of_file(struct parser* parser, char const* filename) { return lexer_of_file(&parser->lexer, filename); }

void parser_dtor(struct parser* parser) { lexer_dtor(&parser->lexer); }

struct ast* parser_parse(struct parser* parser) { return parser_parse_program(parser); }

static inline enum token_type parser_peek_token(struct parser* parser, enum token_type type)
{
	struct token token = lexer_peek(&parser->lexer);
	if (token.type & type) return token.type;
	if (token.type == TOKEN_ERROR) return TOKEN_NONE;
	loc_print(&token.loc, stderr);
	fputs(": invalid token ", stderr);
	token_type_print(token.type, stderr);
	fputs(", expected one of ", stderr);
	token_types_print(type, stderr);
	fputc('\n', stderr);
	parser->error = true;
	return TOKEN_NONE;
}

static inline enum token_type parser_pop_token(struct parser* parser, enum token_type type)
{
	if (!parser_peek_token(parser, type)) return TOKEN_NONE;
	struct token token = lexer_pop(&parser->lexer);
	enum token_type r = token.type;
	token_dtor(&token);
	return r;
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
	DBG_ASSERT((PARSER_FIRST_EXP | PARSER_FIRST_ASSIGN) == PARSER_FIRST_STATEMENT
			   && "parser_parse_statement: missing case in parser");
	DBG_ASSERT((PARSER_FIRST_EXP & PARSER_FIRST_ASSIGN) == 0 && "parser_parse_statement: invalid case in parser");
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_STATEMENT)) return ast_init(AST_ERROR, &token.loc);
	struct ast* statement = NULL;
	if (token.type & PARSER_FIRST_ASSIGN)
		statement = parser_parse_assign(parser);
	else if (token.type & PARSER_FIRST_EXP)
		statement = parser_parse_exp(parser);
	else
		UNREACHABLE();
	if (statement->type == AST_ERROR)
		while (parser_skip_except(parser, TOKEN_SEMICOLON | TOKEN_NEWLINE)) {}
	parser_pop_token(parser, TOKEN_SEMICOLON);
	return statement;
}

static inline struct ast* parser_parse_exp(struct parser* parser)
{
	DBG_ASSERT((PARSER_FIRST_NUMLIT | PARSER_FIRST_STRLIT | PARSER_FIRST_VAR | PARSER_FIRST_OPS | PARSER_FIRST_CALL)
				   == PARSER_FIRST_EXP
			   && "parser_parse_exp: missing case in parser");
	DBG_ASSERT((PARSER_FIRST_NUMLIT & PARSER_FIRST_VAR) == 0 && "parser_parse_exp: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_NUMLIT & PARSER_FIRST_OPS) == 0 && "parser_parse_exp: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_VAR & PARSER_FIRST_OPS) == 0 && "parser_parse_exp: invalid case in parser");
	DBG_ASSERT(PARSER_FIRST_CALL == PARSER_FIRST_VAR && "parser_parse_exp: invalid case in parser");
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_EXP)) return ast_init(AST_ERROR, &token.loc);
	if (token.type & PARSER_FIRST_NUMLIT) return parser_parse_numlit(parser);
	if (token.type & PARSER_FIRST_STRLIT) return parser_parse_strlit(parser);
	if (token.type & PARSER_FIRST_OPS) return parser_parse_ops(parser);
	if (token.type & PARSER_FIRST_VAR)
	{
		struct ast* var = parser_parse_var(parser);
		if (lexer_peek(&parser->lexer).type != TOKEN_LPAREN) return var;
		return parser_parse_call(parser, var);
	}
	UNREACHABLE();
}

static inline struct ast* parser_parse_numlit(struct parser* parser)
{
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, TOKEN_NUMLIT)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct ast* number = ast_init(AST_NUMLIT, &token.loc);
	number->value.numlit.u64 = token.val.u64;
	token_dtor(&token);
	return number;
}

static inline struct ast* parser_parse_strlit(struct parser* parser)
{
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, TOKEN_STRLIT)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct ast* word = ast_init(AST_STRLIT, &token.loc);
	str_move(&word->value.strlit.str, &token.val.str);
	token_dtor(&token);
	return word;
}

static inline struct ast* parser_parse_var(struct parser* parser)
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

static inline struct ast* parser_parse_lexp(struct parser* parser) { return parser_parse_var(parser); }

static inline struct ast* parser_parse_texp(struct parser* parser) { return parser_parse_var(parser); }

static inline struct ast* parser_parse_assign(struct parser* parser)
{
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_LET)) return ast_init(AST_ERROR, &loc);
	struct ast* lexp = parser_parse_lexp(parser);
	enum token_type type = parser_pop_token(parser, TOKEN_COLON | TOKEN_EQUAL);
	if (!type) return ast_init(AST_ERROR, &loc);
	struct ast* texp = NULL;
	if (type == TOKEN_COLON)
	{
		texp = parser_parse_texp(parser);
		parser_pop_token(parser, TOKEN_EQUAL);
	}
	struct ast* exp = parser_parse_exp(parser);
	loc = LOC(loc, exp->loc);
	struct ast* assign = ast_init(AST_ASSIGN, &loc);
	assign->value.assign.texp = texp;
	assign->value.assign.lexp = lexp;
	assign->value.assign.exp = exp;
	return assign;
}

static inline struct ast* parser_parse_call(struct parser* parser, struct ast* lexp)
{
	if (!lexp)
	{
		lexp = parser_parse_lexp(parser);
		parser_pop_token(parser, TOKEN_LPAREN);
	}
	struct loc loc = lexp->loc;
	parser_pop_token(parser, TOKEN_LPAREN);
	struct list args;
	list_ctor(&args, &vlist_past, 16);
	while (lexer_peek(&parser->lexer).type & ~(TOKEN_RPAREN | TOKEN_EOF))
	{
		struct ast* exp = parser_parse_exp(parser);
		loc = LOC(loc, exp->loc);
		list_push_move(&args, &exp);
		if (lexer_peek(&parser->lexer).type != TOKEN_COMA) break;
		lexer_pop(&parser->lexer);
	}
	parser_pop_token(parser, TOKEN_RPAREN);
	struct ast* call = ast_init(AST_CALL, &loc);
	call->value.call.fun = lexp;
	list_move(&call->value.call.args, &args);
	return call;
}
