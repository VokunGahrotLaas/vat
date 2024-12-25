#pragma once

// bootstrap
#include "stream/location.h"
#include "utils/list.h"
#include "utils/str.h"

enum ast_type
{
	AST_ERROR,
	// exps
	AST_NUMLIT,
	AST_STRLIT,
	AST_WORD,
	AST_UNARY,
	AST_CALL,
	// statements
	AST_SEQ,
	AST_SEXP,
	AST_VARDEC,
	AST_FNDEC,
	AST_RET,
};

struct ast_numlit
{
	uint64_t u64;
};

struct ast_strlit
{
	struct str str;
};

struct ast_word
{
	struct str str;
};

enum unary_type
{
	UNARY_PLUS,
	UNARY_MINUS,
};

struct ast_unary
{
	enum unary_type op;
	struct ast* rhs;
};

struct ast_call
{
	struct ast* fun;
	struct list args;
};

struct ast_seq
{
	struct list list;
};

struct ast_sexp
{
	struct ast* exp;
};

struct ast_vardec
{
	struct ast* name;
	struct ast* texp;
	struct ast* exp;
};

struct ast_fndec
{
	struct list args;
	struct list targs;
	struct ast* name;
	struct ast* texp;
	struct ast* exp;
};

struct ast_ret
{
	struct ast* exp;
};

struct ast
{
	enum ast_type type;
	struct loc loc;
	union ast_value
	{
		struct ast_numlit numlit;
		struct ast_strlit strlit;
		struct ast_word word;
		struct ast_unary unary;
		struct ast_call call;
		struct ast_seq seq;
		struct ast_sexp sexp;
		struct ast_vardec vardec;
		struct ast_fndec fndec;
		struct ast_ret ret;
	} value;
};

struct ast* ast_init(enum ast_type type, struct loc const* loc);
void ast_free(struct ast* ast);
void ast_pdtor(struct ast** ast);
void ast_print(struct ast* ast, FILE* stream);

void unary_print(enum unary_type type, FILE* stream);

bool seq_push(struct ast* seq, struct ast* ast);

VLIST(struct ast*, vlist_past, &ast_pdtor, &copy_fail, NULL);
