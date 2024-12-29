#pragma once

// bootstrap
#include "lexer/location.h"
#include "utils/dict.h"
#include "utils/list.h"
#include "utils/str.h"

enum ast_type
{
	AST_ERROR,
	// exps
	AST_NUMLIT,
	AST_STRLIT,
	AST_VAR,
	AST_UNARY,
	AST_CALL,
	AST_ATTR,
	// statements
	AST_SEQ,
	AST_SEXP,
	AST_MODDEC,
	AST_IMPDEC,
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

struct ast_var
{
	struct str name;
	struct ast* next;
	struct ast* dec;
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
	struct ast* name;
	struct list args;
};

struct ast_attr
{
	struct ast* name;
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

struct ast_moddec
{
	struct ast* name;
};

struct ast_impdec
{
	struct ast* mod_name;
	struct ast* as_name;
	struct module* module;
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
	struct ast* name;
	struct ast* texp;
	struct ast* exp;
};

struct ast_ret
{
	struct ast* exp;
};

struct type;

struct ast
{
	union ast_value
	{
		struct ast_numlit numlit;
		struct ast_strlit strlit;
		struct ast_var var;
		struct ast_unary unary;
		struct ast_call call;
		struct ast_attr attr;
		struct ast_seq seq;
		struct ast_sexp sexp;
		struct ast_moddec moddec;
		struct ast_impdec impdec;
		struct ast_vardec vardec;
		struct ast_fndec fndec;
		struct ast_ret ret;
	} value;
	struct dict attrs;
	struct loc loc;
	struct type* type;
	enum ast_type ast_type;
};

struct ast* ast_init(enum ast_type type, struct loc const* loc);
void ast_free(struct ast* ast);
void ast_pdtor(struct ast** ast);
void ast_print(struct ast* ast, FILE* stream);

void unary_print(enum unary_type type, FILE* stream);

bool seq_push(struct ast* seq, struct ast* ast);

// impl

VTYPE(vtype_past, struct ast*, NULL, NULL, NULL, NULL, NULL, NULL);
VTYPE(vtype_upast, struct ast*, &ast_pdtor, &copy_fail, NULL, NULL, NULL, NULL);
VLIST(vlist_past, struct ast*, &vtype_past);
VLIST(vlist_upast, struct ast*, &vtype_upast);

VPAIR(vpair_str_upast, &vtype_str, &vtype_upast);
VDICT(vdict_str_upast, struct str, struct ast*, &vpair_str_upast);
