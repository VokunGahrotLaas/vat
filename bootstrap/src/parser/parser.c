#include "parser/parser.h"

#define PARSER_FIRST_PROGRAM (PARSER_FIRST_STATEMENTS)
#define PARSER_FIRST_STATEMENTS (PARSER_FIRST_STATEMENT)
#define PARSER_FIRST_STATEMENT (PARSER_FIRST_STATEMENT1 | PARSER_FIRST_ATTRS)
#define PARSER_FIRST_STATEMENT1                                                                                        \
	(PARSER_FIRST_EXP | PARSER_FIRST_MODDEC | PARSER_FIRST_IMPDEC | PARSER_FIRST_VARDEC | PARSER_FIRST_FNDEC           \
	 | PARSER_FIRST_RET)
#define PARSER_FIRST_EXP                                                                                               \
	(PARSER_FIRST_NUMLIT | PARSER_FIRST_STRLIT | PARSER_FIRST_VAR | PARSER_FIRST_OPS | PARSER_FIRST_CALL)
#define PARSER_FIRST_SEXP (PARSER_FIRST_EXP)
#define PARSER_FIRST_NUMLIT (TOKEN_NUMLIT)
#define PARSER_FIRST_STRLIT (TOKEN_STRLIT)
#define PARSER_FIRST_VAR (TOKEN_WORD)
#define PARSER_FIRST_OPS (TOKEN_LPAREN | TOKEN_PLUS | TOKEN_MINUS)
#define PARSER_FIRST_TEXP (PARSER_FIRST_VAR)
#define PARSER_FIRST_MODDEC (TOKEN_MODULE)
#define PARSER_FIRST_IMPDEC (TOKEN_IMPORT)
#define PARSER_FIRST_VARDEC (TOKEN_LET)
#define PARSER_FIRST_CALL (PARSER_FIRST_VAR)
#define PARSER_FIRST_FNDEC (TOKEN_FN)
#define PARSER_FIRST_RET (TOKEN_RET)
#define PARSER_FIRST_ATTRS (PARSER_FIRST_ATTR)
#define PARSER_FIRST_ATTR (TOKEN_AT)

static inline enum token_type parser_peek_token(struct parser* parser, enum token_type type);
static inline enum token_type parser_pop_token(struct parser* parser, enum token_type type);
static inline void parser_skip_except(struct parser* parser, enum token_type type);
static inline void parser_skip_whitespace(struct parser* parser);

static inline struct ast* parser_parse_program(struct parser* parser);
static inline struct ast* parser_parse_statements(struct parser* parser);
static inline struct ast* parser_parse_statement(struct parser* parser);
static inline struct ast* parser_parse_exp(struct parser* parser);
static inline struct ast* parser_parse_sexp(struct parser* parser);
static inline struct ast* parser_parse_numlit(struct parser* parser);
static inline struct ast* parser_parse_strlit(struct parser* parser);
static inline struct ast* parser_parse_var(struct parser* parser);
static inline struct ast* parser_parse_mvar(struct parser* parser);
static inline struct ast* parser_parse_ops(struct parser* parser);
static inline struct ast* parser_parse_texp(struct parser* parser);
static inline struct ast* parser_parse_vardec(struct parser* parser);
static inline struct ast* parser_parse_call(struct parser* parser, struct ast* var);
static inline struct ast* parser_parse_fndec(struct parser* parser);
static inline struct ast* parser_parse_ret(struct parser* parser);
static inline struct dict parser_parse_attrs(struct parser* parser);
static inline struct ast* parser_parse_attr(struct parser* parser);
static inline struct ast* parser_parse_moddec(struct parser* parser);
static inline struct ast* parser_parse_impdec(struct parser* parser);

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

static inline void parser_skip(struct parser* parser, enum token_type type)
{
	while (lexer_peek(&parser->lexer).type & type)
	{
		struct token token = lexer_pop(&parser->lexer);
		token_dtor(&token);
	}
}

static inline void parser_skip_except(struct parser* parser, enum token_type type)
{
	type |= TOKEN_EOF;
	parser_skip(parser, ~type);
}

static inline void parser_skip_whitespace(struct parser* parser)
{
	parser_skip(parser, TOKEN_WHITESPACE | TOKEN_NEWLINE);
}

static inline struct ast* parser_parse_program(struct parser* parser)
{
	struct loc loc = lexer_peek(&parser->lexer).loc;
	struct ast* ast = ast_init(AST_SEQ, &loc);
	list_ctor(&ast->value.seq.list, &vlist_upast, 16);
	while (lexer_peek(&parser->lexer).type & ~TOKEN_EOF)
	{
		struct ast* statements = parser_parse_statements(parser);
		if (statements->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_NEWLINE);
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
	list_ctor(&ast->value.seq.list, &vlist_upast, 16);
	parser_skip(parser, TOKEN_WHITESPACE);
	while (lexer_peek(&parser->lexer).type & ~(TOKEN_EOF | TOKEN_NEWLINE | TOKEN_RCURLBRA))
	{
		struct ast* statement = parser_parse_statement(parser);
		if (statement->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_NEWLINE | TOKEN_RCURLBRA);
		seq_push(ast, statement);
		parser_skip(parser, TOKEN_WHITESPACE);
	}
	if (ast->value.seq.list.size > 0) ast->loc = LOC(ast->loc, (*LIST_BACK(&ast->value.seq.list, struct ast*))->loc);
	return ast;
}

static inline struct ast* parser_parse_statement(struct parser* parser)
{
	DBG_ASSERT((PARSER_FIRST_SEXP | PARSER_FIRST_MODDEC | PARSER_FIRST_IMPDEC | PARSER_FIRST_VARDEC | PARSER_FIRST_FNDEC
				| PARSER_FIRST_RET | PARSER_FIRST_ATTRS)
				   == PARSER_FIRST_STATEMENT
			   && "parser_parse_statement: missing case in parser");
	DBG_ASSERT((PARSER_FIRST_SEXP & PARSER_FIRST_VARDEC) == 0 && "parser_parse_statement: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_SEXP & PARSER_FIRST_FNDEC) == 0 && "parser_parse_statement: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_SEXP & PARSER_FIRST_RET) == 0 && "parser_parse_statement: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_VARDEC & PARSER_FIRST_FNDEC) == 0 && "parser_parse_statement: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_VARDEC & PARSER_FIRST_RET) == 0 && "parser_parse_statement: invalid case in parser");
	DBG_ASSERT((PARSER_FIRST_FNDEC & PARSER_FIRST_RET) == 0 && "parser_parse_statement: invalid case in parser");

	parser_skip_whitespace(parser);
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_STATEMENT)) return ast_init(AST_ERROR, &token.loc);
	struct dict attrs;
	bool has_attrs = false;
	if (token.type & PARSER_FIRST_ATTRS)
	{
		has_attrs = true;
		attrs = parser_parse_attrs(parser);
	}

	parser_skip_whitespace(parser);
	token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_STATEMENT1))
	{
		if (has_attrs) dict_dtor(&attrs);
		return ast_init(AST_ERROR, &token.loc);
	}
	struct ast* statement = NULL;
	if (token.type & PARSER_FIRST_VARDEC)
		statement = parser_parse_vardec(parser);
	else if (token.type & PARSER_FIRST_FNDEC)
		statement = parser_parse_fndec(parser);
	else if (token.type & PARSER_FIRST_RET)
		statement = parser_parse_ret(parser);
	else if (token.type & PARSER_FIRST_SEXP)
		statement = parser_parse_sexp(parser);
	else if (token.type & PARSER_FIRST_MODDEC)
		statement = parser_parse_moddec(parser);
	else if (token.type & PARSER_FIRST_IMPDEC)
		statement = parser_parse_impdec(parser);
	else
		UNREACHABLE();
	if (statement->ast_type == AST_ERROR)
	{
		parser_skip_except(parser, TOKEN_SEMICOLON | TOKEN_RCURLBRA);
		parser_pop_token(parser, TOKEN_SEMICOLON | TOKEN_RCURLBRA);
	}
	if (has_attrs)
	{
		dict_dtor(&statement->attrs);
		vmove(&vtype_dict, &statement->attrs, &attrs);
	}
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
	parser_skip_whitespace(parser);
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_EXP)) return ast_init(AST_ERROR, &token.loc);
	if (token.type & PARSER_FIRST_NUMLIT) return parser_parse_numlit(parser);
	if (token.type & PARSER_FIRST_STRLIT) return parser_parse_strlit(parser);
	if (token.type & PARSER_FIRST_OPS) return parser_parse_ops(parser);
	if (token.type & PARSER_FIRST_VAR)
	{
		struct ast* var = parser_parse_mvar(parser);
		parser_skip_whitespace(parser);
		if (lexer_peek(&parser->lexer).type != TOKEN_LPAREN) return var;
		return parser_parse_call(parser, var);
	}
	UNREACHABLE();
}

static inline struct ast* parser_parse_sexp(struct parser* parser)
{
	struct ast* exp = parser_parse_exp(parser);
	parser_skip_whitespace(parser);
	if (exp->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_SEMICOLON | TOKEN_RCURLBRA);
	parser_pop_token(parser, TOKEN_SEMICOLON);
	struct ast* sexp = ast_init(AST_SEXP, &exp->loc);
	sexp->value.sexp.exp = exp;
	return sexp;
}

static inline struct ast* parser_parse_numlit(struct parser* parser)
{
	parser_skip_whitespace(parser);
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
	parser_skip_whitespace(parser);
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, TOKEN_STRLIT)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct ast* word = ast_init(AST_STRLIT, &token.loc);
	vmove(&vtype_str, &word->value.strlit.str, &token.val.str);
	token_dtor(&token);
	return word;
}

static inline struct ast* parser_parse_mvar(struct parser* parser)
{
	struct ast* mvar = parser_parse_var(parser);
	struct ast* last_var = mvar;
	parser_skip_whitespace(parser);
	while (lexer_peek(&parser->lexer).type == TOKEN_DOT)
	{
		lexer_pop(&parser->lexer);
		struct ast* var = parser_parse_var(parser);
		last_var->value.var.next = var;
		last_var = var;
		parser_skip_whitespace(parser);
	}
	return mvar;
}

static inline struct ast* parser_parse_var(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, TOKEN_WORD)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	struct ast* var = ast_init(AST_VAR, &token.loc);
	vmove(&vtype_str, &var->value.var.name, &token.val.str);
	var->value.var.dec = NULL;
	var->value.var.next = NULL;
	token_dtor(&token);
	return var;
}

static inline struct ast* parser_parse_ops(struct parser* parser)
{
	DBG_ASSERT((TOKEN_LPAREN | TOKEN_PLUS | TOKEN_MINUS) == PARSER_FIRST_OPS
			   && "parser_parse_ops: missing case in parser");
	parser_skip_whitespace(parser);
	struct token token = lexer_peek(&parser->lexer);
	if (!parser_peek_token(parser, PARSER_FIRST_OPS)) return ast_init(AST_ERROR, &token.loc);
	lexer_pop(&parser->lexer);
	parser_skip_whitespace(parser);
	struct loc loc = token.loc;
	enum token_type type = token.type;
	token_dtor(&token);
	if (type == TOKEN_LPAREN)
	{
		struct ast* exp = parser_parse_exp(parser);
		// not sure if i should prioritize error detection for missing RPAREN and skip to RPAREN | NEWLINE
		// of this solution that skips to RPAREN in case of an error inside parens
		// TODO: find a way to cover both cases
		if (exp->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_RPAREN);
		parser_skip_whitespace(parser);
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

static inline struct ast* parser_parse_texp(struct parser* parser) { return parser_parse_mvar(parser); }

static inline struct ast* parser_parse_vardec(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_LET)) return ast_init(AST_ERROR, &loc);
	parser_skip_whitespace(parser);
	struct ast* name = parser_parse_var(parser);
	parser_skip_whitespace(parser);
	enum token_type type = parser_pop_token(parser, TOKEN_COLON | TOKEN_EQUAL);
	if (!type) return ast_init(AST_ERROR, &loc);
	struct ast* texp = NULL;
	if (type == TOKEN_COLON)
	{
		texp = parser_parse_texp(parser);
		parser_skip_whitespace(parser);
		parser_pop_token(parser, TOKEN_EQUAL);
	}
	struct ast* exp = parser_parse_exp(parser);
	parser_skip_whitespace(parser);
	if (exp->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_SEMICOLON | TOKEN_RCURLBRA);
	parser_pop_token(parser, TOKEN_SEMICOLON);
	loc = LOC(loc, exp->loc);
	struct ast* vardec = ast_init(AST_VARDEC, &loc);
	vardec->value.vardec.name = name;
	vardec->value.vardec.texp = texp;
	vardec->value.vardec.exp = exp;
	return vardec;
}

static inline struct ast* parser_parse_call(struct parser* parser, struct ast* var)
{
	if (!var)
	{
		var = parser_parse_mvar(parser);
		parser_skip_whitespace(parser);
		parser_pop_token(parser, TOKEN_LPAREN);
	}
	struct loc loc = var->loc;
	parser_skip_whitespace(parser);
	parser_pop_token(parser, TOKEN_LPAREN);
	struct list args;
	list_ctor(&args, &vlist_upast, 16);
	parser_skip_whitespace(parser);
	while (lexer_peek(&parser->lexer).type & ~(TOKEN_RPAREN | TOKEN_EOF))
	{
		struct ast* exp = parser_parse_exp(parser);
		loc = LOC(loc, exp->loc);
		list_push_move(&args, &exp);
		parser_skip_whitespace(parser);
		if (lexer_peek(&parser->lexer).type != TOKEN_COMA) break;
		lexer_pop(&parser->lexer);
		parser_skip_whitespace(parser);
	}
	parser_pop_token(parser, TOKEN_RPAREN);
	struct ast* call = ast_init(AST_CALL, &loc);
	call->value.call.name = var;
	vmove(&vtype_list, &call->value.call.args, &args);
	return call;
}

static inline struct ast* parser_parse_fndec(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_FN)) return ast_init(AST_ERROR, &loc);
	parser_skip_whitespace(parser);
	struct ast* name = parser_parse_var(parser);
	parser_skip_whitespace(parser);
	if (!parser_pop_token(parser, TOKEN_LPAREN)) return ast_init(AST_ERROR, &loc);
	parser_skip_whitespace(parser);
	struct list args;
	struct list targs;
	list_ctor(&args, &vlist_upast, 16);
	list_ctor(&targs, &vlist_upast, 16);
	while (lexer_peek(&parser->lexer).type & ~(TOKEN_RPAREN | TOKEN_EOF))
	{
		struct ast* targ = NULL;
		struct ast* arg = parser_parse_var(parser);
		parser_skip_whitespace(parser);
		enum token_type type = parser_peek_token(parser, TOKEN_COLON | TOKEN_COMA | TOKEN_RPAREN);
		if (!type) return ast_init(AST_ERROR, &loc);
		if (type == TOKEN_COLON)
		{
			lexer_pop(&parser->lexer);
			parser_skip_whitespace(parser);
			targ = parser_parse_texp(parser);
			parser_skip_whitespace(parser);
			type = parser_peek_token(parser, TOKEN_COMA | TOKEN_RPAREN);
			if (!type) return ast_init(AST_ERROR, &loc);
		}
		if (type == TOKEN_COMA) lexer_pop(&parser->lexer);
		list_push_move(&args, &arg);
		list_push_move(&targs, &targ);
	}
	if (!parser_pop_token(parser, TOKEN_RPAREN)) return ast_init(AST_ERROR, &loc);
	parser_skip_whitespace(parser);
	struct ast* texp = NULL;
	if (lexer_peek(&parser->lexer).type == TOKEN_ARROW)
	{
		lexer_pop(&parser->lexer);
		parser_skip_whitespace(parser);
		texp = parser_parse_texp(parser);
		parser_skip_whitespace(parser);
	}
	struct ast* exp = NULL;
	if (lexer_peek(&parser->lexer).type == TOKEN_LCURLBRA)
	{
		struct token token = lexer_pop(&parser->lexer);
		struct loc loc = token.loc;
		parser_skip_whitespace(parser);
		struct ast* seq = ast_init(AST_SEQ, &loc);
		list_ctor(&seq->value.seq.list, &vlist_upast, 16);
		while (lexer_peek(&parser->lexer).type & ~(TOKEN_RCURLBRA | TOKEN_EOF))
		{
			struct ast* statements = parser_parse_statements(parser);
			if (statements->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_RCURLBRA | TOKEN_NEWLINE);
			seq_push(seq, statements);
			parser_skip(parser, TOKEN_WHITESPACE);
			if (lexer_peek(&parser->lexer).type == TOKEN_NEWLINE) lexer_pop(&parser->lexer);
		}
		parser_skip_whitespace(parser);
		seq->loc = LOC(seq->loc, lexer_peek(&parser->lexer).loc);
		if (!parser_pop_token(parser, TOKEN_RCURLBRA)) return ast_init(AST_ERROR, &loc);
		exp = seq;
	}
	else if (lexer_peek(&parser->lexer).type == TOKEN_SEMICOLON)
		lexer_pop(&parser->lexer);
	else
		exp = parser_parse_statement(parser);
	if (exp != NULL) loc = LOC(loc, exp->loc);
	struct ast* fndec = ast_init(AST_FNDEC, &loc);
	vmove(&vtype_list, &fndec->value.fndec.args, &args);
	vmove(&vtype_list, &fndec->value.fndec.targs, &targs);
	fndec->value.fndec.name = name;
	fndec->value.fndec.texp = texp;
	fndec->value.fndec.exp = exp;
	return fndec;
}

static inline struct ast* parser_parse_ret(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_RET)) return ast_init(AST_ERROR, &loc);
	parser_skip_whitespace(parser);
	struct ast* exp = parser_parse_exp(parser);
	parser_skip_whitespace(parser);
	if (exp->ast_type == AST_ERROR) parser_skip_except(parser, TOKEN_SEMICOLON | TOKEN_RCURLBRA);
	parser_pop_token(parser, TOKEN_SEMICOLON);
	loc = LOC(loc, exp->loc);
	struct ast* assign = ast_init(AST_RET, &loc);
	assign->value.ret.exp = exp;
	return assign;
}

static inline struct dict parser_parse_attrs(struct parser* parser)
{
	struct dict attrs;
	dict_ctor(&attrs, &vdict_str_upast, 1);
	struct ast* attr = parser_parse_attr(parser);
	struct str name;
	str_ctor(&name, 16);
	for (struct ast* var = attr->value.attr.name; var != NULL; var = var->value.var.next)
	{
		if (var != attr->value.attr.name) str_pushc(&name, '.');
		str_pushcv(&name, cv_str(&var->value.var.name));
	}
	dict_add_move(&attrs, &name, &attr);
	parser_skip_whitespace(parser);
	while (lexer_peek(&parser->lexer).type == TOKEN_AT)
	{
		attr = parser_parse_attr(parser);
		struct str name;
		str_ctor(&name, 16);
		for (struct ast* var = attr->value.attr.name; var != NULL; var = var->value.var.next)
		{
			if (var != attr->value.attr.name) str_pushc(&name, '.');
			str_pushcv(&name, cv_str(&var->value.var.name));
		}
		dict_add_move(&attrs, &name, &attr);
		parser_skip_whitespace(parser);
	}
	return attrs;
}

static inline struct ast* parser_parse_attr(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_AT)) return ast_init(AST_ERROR, &loc);
	struct ast* var = parser_parse_mvar(parser);
	parser_skip_whitespace(parser);
	struct list args;
	list_ctor(&args, &vlist_upast, 16);
	if (lexer_peek(&parser->lexer).type == TOKEN_LPAREN)
	{
		lexer_pop(&parser->lexer);
		parser_skip_whitespace(parser);
		while (lexer_peek(&parser->lexer).type & ~(TOKEN_RPAREN | TOKEN_EOF))
		{
			struct ast* exp = parser_parse_exp(parser);
			loc = LOC(loc, exp->loc);
			list_push_move(&args, &exp);
			parser_skip_whitespace(parser);
			if (lexer_peek(&parser->lexer).type != TOKEN_COMA) break;
			lexer_pop(&parser->lexer);
			parser_skip_whitespace(parser);
		}
		parser_pop_token(parser, TOKEN_RPAREN);
	}
	struct ast* call = ast_init(AST_ATTR, &loc);
	call->value.attr.name = var;
	vmove(&vtype_list, &call->value.attr.args, &args);
	return call;
}

static inline struct ast* parser_parse_moddec(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_MODULE)) return ast_init(AST_ERROR, &loc);
	struct ast* name = parser_parse_mvar(parser);
	loc = LOC(loc, name->loc);
	parser_skip_whitespace(parser);
	parser_pop_token(parser, TOKEN_SEMICOLON);
	struct ast* moddec = ast_init(AST_MODDEC, &loc);
	moddec->value.moddec.name = name;
	return moddec;
}

static inline struct ast* parser_parse_impdec(struct parser* parser)
{
	parser_skip_whitespace(parser);
	struct loc loc = lexer_peek(&parser->lexer).loc;
	if (!parser_pop_token(parser, TOKEN_IMPORT)) return ast_init(AST_ERROR, &loc);
	struct ast* mod_name = parser_parse_mvar(parser);
	loc = LOC(loc, mod_name->loc);
	parser_skip_whitespace(parser);
	struct ast* as_name = NULL;
	if (lexer_peek(&parser->lexer).type == TOKEN_AS)
	{
		lexer_pop(&parser->lexer);
		as_name = parser_parse_mvar(parser);
		loc = LOC(loc, as_name->loc);
		parser_skip_whitespace(parser);
	}
	parser_pop_token(parser, TOKEN_SEMICOLON);
	struct ast* moddec = ast_init(AST_MODDEC, &loc);
	moddec->value.impdec.mod_name = mod_name;
	moddec->value.impdec.as_name = as_name;
	return moddec;
}
