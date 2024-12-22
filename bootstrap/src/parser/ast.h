#pragma once

// bootstrap
#include "stream/location.h"
#include "utils/list.h"

enum ast_type
{
	AST_NUMBER,
	AST_UNARY,
	AST_SEQ,
};

struct ast_value_number
{
	uint64_t u64;
};

enum unary_type
{
	UNARY_PLUS,
	UNARY_MINUS,
};

struct ast_value_unary
{
	enum unary_type op;
	struct ast* rhs;
};

struct ast_value_seq
{
	struct list list;
};

struct ast
{
	enum ast_type type;
	struct loc loc;
	union ast_value
	{
		struct ast_value_number number;
		struct ast_value_unary unary;
		struct ast_value_seq seq;
	} value;
};

struct ast* ast_init(enum ast_type type, struct loc const* loc);
void ast_free(struct ast* ast);
void ast_pdtor(struct ast** ast);

void unary_print(enum unary_type type, FILE* stream);
void ast_print(struct ast* ast, FILE* stream);

VLIST(struct ast*, vlist_past, &ast_pdtor, NULL, NULL);
