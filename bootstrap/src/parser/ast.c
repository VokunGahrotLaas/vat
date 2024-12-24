#include "parser/ast.h"

// libc
#include <stdlib.h>

static inline void ast_print_indent(FILE* stream, size_t indent);
static inline void ast_print_impl(struct ast* ast, FILE* stream, size_t indent);

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
	case AST_NUMLIT: break;
	case AST_STRLIT: str_dtor(&ast->value.word.str); break;
	case AST_WORD: str_dtor(&ast->value.word.str); break;
	case AST_UNARY: ast_free(ast->value.unary.rhs); break;
	case AST_CALL:
		ast_free(ast->value.call.fun);
		list_dtor(&ast->value.call.args);
		break;
	case AST_SEQ: list_dtor(&ast->value.seq.list); break;
	case AST_ASSIGN: {
		struct ast_assign* assign = &ast->value.assign;
		ast_free(assign->texp);
		ast_free(assign->lexp);
		ast_free(assign->exp);
		break;
	}
	};
	free(ast);
}

void ast_pdtor(struct ast** ast)
{
	if (!ast) return;
	ast_free(*ast);
	*ast = NULL;
}

void ast_print(struct ast* ast, FILE* stream) { ast_print_impl(ast, stream, 0); }

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

static inline void ast_print_indent(FILE* stream, size_t indent)
{
	for (size_t i = 0; i < indent; ++i)
		fputc('\t', stream);
}

static inline void ast_print_impl(struct ast* ast, FILE* stream, size_t indent)
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
	case AST_NUMLIT: fprintf(stream, "%" PRIu64, ast->value.numlit.u64); break;
	case AST_STRLIT:
		fputc('"', stream);
		cv_print(cv_str(&ast->value.strlit.str), stream);
		fputc('"', stream);
		break;
	case AST_WORD: cv_print(cv_str(&ast->value.word.str), stream); break;
	case AST_CALL:
		ast_print(ast->value.call.fun, stream);
		fputc('(', stream);
		struct list* args = &ast->value.call.args;
		for (size_t i = 0; i < args->size; ++i)
		{
			if (i != 0) fputs(", ", stream);
			ast_print_impl(*LIST_GET(args, struct ast*, i), stream, indent);
		}
		fputc(')', stream);
		break;
	case AST_UNARY:
		unary_print(ast->value.unary.op, stream);
		ast_print(ast->value.unary.rhs, stream);
		break;
	case AST_SEQ: {
		struct list* list = &ast->value.seq.list;
		for (size_t i = 0; i < list->size; ++i)
		{
			ast_print_indent(stream, indent);
			ast_print_impl(*LIST_GET(list, struct ast*, i), stream, indent);
			fputs(";\n", stream);
		}
		break;
	}
	case AST_ASSIGN: {
		struct ast_assign* assign = &ast->value.assign;
		fputs("let ", stream);
		ast_print_impl(assign->lexp, stream, indent);
		if (assign->texp)
		{
			fputs(" : ", stream);
			ast_print_impl(assign->texp, stream, indent);
		}
		fputs(" = ", stream);
		ast_print_impl(assign->exp, stream, indent);
		break;
	}
	};
}
