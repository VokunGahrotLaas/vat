#include "parser/ast.h"

// libc
#include <stdlib.h>
// bootstrap
#include "post/type.h"

static inline void ast_print_indent(FILE* stream, size_t indent);
static inline void ast_print_impl(struct ast* ast, FILE* stream, size_t indent);

struct ast* ast_init(enum ast_type type, struct loc const* loc)
{
	struct ast* ast = calloc(1, sizeof(*ast));
	if (!ast) return NULL;
	ast->loc = loc ? *loc : LOC_INVALID;
	ast->ast_type = type;
	ast->type = NULL;
	return ast;
}

void ast_free(struct ast* ast)
{
	if (!ast) return;
	switch (ast->ast_type)
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
	case AST_SEXP: ast_free(ast->value.sexp.exp); break;
	case AST_VARDEC: {
		struct ast_vardec* vardec = &ast->value.vardec;
		ast_free(vardec->name);
		ast_free(vardec->texp);
		ast_free(vardec->exp);
		break;
	}
	case AST_FNDEC: {
		struct ast_fndec* fndec = &ast->value.fndec;
		list_dtor(&fndec->args);
		list_dtor(&fndec->targs);
		ast_free(fndec->name);
		ast_free(fndec->texp);
		ast_free(fndec->exp);
		break;
	}
	case AST_RET: ast_free(ast->value.ret.exp); break;
	};
	type_free(ast->type);
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
	if (ast->ast_type != AST_SEQ) return list_push_move(&seq->value.seq.list, &ast);
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
	switch (ast->ast_type)
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
	case AST_WORD:
		cv_print(cv_str(&ast->value.word.str), stream);
		if (ast->value.word.dec == NULL) break;
		fprintf(stream, " /* %p */", (void*)ast->value.word.dec);
		break;
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
			struct ast* exp = *LIST_GET(list, struct ast*, i);
			ast_print_impl(exp, stream, indent);
			fputc('\n', stream);
		}
		break;
	}
	case AST_SEXP:
		ast_print_impl(ast->value.sexp.exp, stream, indent);
		fputc(';', stream);
		break;
	case AST_VARDEC: {
		struct ast_vardec* vardec = &ast->value.vardec;
		fputs("let ", stream);
		ast_print_impl(vardec->name, stream, indent);
		if (vardec->texp)
		{
			fputs(": ", stream);
			ast_print_impl(vardec->texp, stream, indent);
		}
		fputs(" = ", stream);
		ast_print_impl(vardec->exp, stream, indent);
		fputc(';', stream);
		break;
	}
	case AST_FNDEC: {
		struct ast_fndec* fndec = &ast->value.fndec;
		fputs("fn ", stream);
		ast_print_impl(fndec->name, stream, indent);
		fputc('(', stream);
		for (size_t i = 0; i < fndec->args.size; ++i)
		{
			if (i != 0) fputs(", ", stream);
			ast_print_impl(*LIST_GET(&fndec->args, struct ast*, i), stream, indent);
			struct ast* texp = *LIST_GET(&fndec->targs, struct ast*, i);
			if (!texp) continue;
			fputs(": ", stream);
			ast_print_impl(texp, stream, indent);
		}
		fputc(')', stream);
		if (fndec->texp)
		{
			fputs(" -> ", stream);
			ast_print_impl(fndec->texp, stream, indent);
		}
		if (fndec->exp->ast_type == AST_SEQ)
		{
			fputs(" {\n", stream);
			ast_print_impl(fndec->exp, stream, indent + 1);
			ast_print_indent(stream, indent);
			fputc('}', stream);
		}
		else
		{
			fputc(' ', stream);
			ast_print_impl(fndec->exp, stream, indent);
		}
		break;
	}
	case AST_RET:
		fputs("ret ", stream);
		ast_print_impl(ast->value.ret.exp, stream, indent);
		fputc(';', stream);
		break;
	};
}
