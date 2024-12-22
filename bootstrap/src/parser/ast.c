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
	case AST_ERROR: break;
	case AST_NUMBER: break;
	case AST_WORD: str_dtor(&ast->value.word.str); break;
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

void ast_print(struct ast* ast, FILE* stream)
{
	if (!ast)
	{
		fputs("<NULL>", stream);
		return;
	}
	switch (ast->type)
	{
	case AST_ERROR:
		fputs("@error(", stream);
		loc_print(&ast->loc, stream);
		fputc(')', stream);
		break;
	case AST_NUMBER: fprintf(stream, "%" PRIu64, ast->value.number.u64); break;
	case AST_WORD: cv_print(cv_str(&ast->value.word.str), stream); break;
	case AST_UNARY:
		unary_print(ast->value.unary.op, stream);
		ast_print(ast->value.unary.rhs, stream);
		break;
	case AST_SEQ: {
		struct list* list = &ast->value.seq.list;
		for (size_t i = 0; i < list->size; ++i)
		{
			ast_print(*LIST_GET(list, struct ast*, i), stream);
			fputs(";\n", stream);
		}
		break;
	}
	};
}

void unary_print(enum unary_type type, FILE* stream)
{
	switch (type)
	{
	case UNARY_PLUS: fputc('+', stream); break;
	case UNARY_MINUS: fputc('-', stream); break;
	};
}

bool seq_push(struct ast* seq, struct ast* ast)
{
	if (ast->type != AST_SEQ) return list_push_move(&seq->value.seq.list, &ast);
	struct list* dest = &seq->value.seq.list;
	struct list* src = &ast->value.seq.list;
	if (!list_reserve(dest, dest->size + src->size))
	{
		ast_free(ast);
		return false;
	}
	for (size_t i = 0; i < src->size; ++i)
	{
		if (list_push_move(dest, list_get(src, i))) continue;
		ast_free(ast);
		return false;
	}
	free(src->data);
	src->data = src->vlist->empty_impl;
	src->capacity = src->size = 0;
	ast_free(ast);
	return true;
}
