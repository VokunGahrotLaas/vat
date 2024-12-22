#include "parser/ast.h"

// libc
#include <stdlib.h>

struct ast* ast_init(enum ast_type type, struct loc const* loc)
{
	struct ast* ast = calloc(1, sizeof(*ast));
	if (!ast) return NULL;
	ast->loc = loc ? *loc : LOC_INVALID;
	ast->type = type;
	return ast;
}

void ast_free(struct ast* ast)
{
	if (!ast) return;
	switch (ast->type)
	{
	case AST_NUMBER: break;
	case AST_UNARY: ast_free(ast->value.unary.rhs); break;
	case AST_SEQ: list_dtor(&ast->value.seq.list); break;
	};
	free(ast);
}

void ast_pdtor(struct ast** ast)
{
	if (!ast) return;
	ast_free(*ast);
	*ast = NULL;
}

void unary_print(enum unary_type type, FILE* stream)
{
	switch (type)
	{
	case UNARY_PLUS: fputc('+', stream); break;
	case UNARY_MINUS: fputc('-', stream); break;
	};
}

void ast_print(struct ast* ast, FILE* stream)
{
	if (!ast)
	{
		fputs("<NULL>", stream);
		return;
	}
	switch (ast->type)
	{
	case AST_NUMBER: fprintf(stream, "%" PRIu64, ast->value.number.u64); break;
	case AST_UNARY:
		unary_print(ast->value.unary.op, stream);
		ast_print(ast->value.unary.rhs, stream);
		break;
	case AST_SEQ: {
		struct list* list = &ast->value.seq.list;
		for (size_t i = 0; i < list->size; ++i)
		{
			ast_print(list_get(list, i), stream);
			fputs(";\n", stream);
		}
		break;
	}
	};
}
