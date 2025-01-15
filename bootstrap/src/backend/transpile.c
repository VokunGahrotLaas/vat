#include "backend/transpile.h"

// libc
#include <sys/mman.h>
#include <sys/sendfile.h>

static inline void transpile_c_newline_indent(FILE* stream, size_t indent);
static inline bool transpile_c_ast(struct dict* modules, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_fndec(struct dict* modules, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_vardec(struct dict* modules, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_builtin_var(struct cv name, FILE* stream);

bool transpile_c_prelude(FILE* stream, UNUSED struct dict* modules)
{
	fputs("#include <stdint.h>\n#include <stddef.h>\n#include <sys/types.h>\n\n", stream);
	return true;
}

bool transpile_c_exports(struct ast* ast, FILE* stream, struct dict* modules)
{
	struct list* list = &ast->value.seq.list;
	for (size_t i = 0; i < list->size; ++i)
	{
		struct ast* exp = *LIST_GET(list, struct ast*, i);
		struct cv expo = cv_cstr("export");
		if (dict_find(&exp->attrs, &expo) == NULL) continue;
		if (exp->ast_type == AST_FNDEC)
		{
			fputs("extern ", stream);
			transpile_c_fndec(modules, exp, 0, stream);
			fputs(";\n", stream);
		}
		else if (exp->ast_type == AST_VARDEC)
		{
			fputs("extern ", stream);
			transpile_c_vardec(modules, exp, 0, stream);
			fputs(";\n", stream);
		}
	}
	return true;
}

bool transpile_c_symbols(struct ast* ast, FILE* stream, struct dict* modules)
{
	struct list* list = &ast->value.seq.list;
	for (size_t i = 0; i < list->size; ++i)
	{
		struct ast* exp = *LIST_GET(list, struct ast*, i);
		struct cv expo = cv_cstr("export");
		struct cv ext = cv_cstr("extern");
		if (exp->ast_type == AST_FNDEC)
		{
			if (dict_find(&exp->attrs, &ext) != NULL)
				fputs("extern ", stream);
			else if (dict_find(&exp->attrs, &expo) == NULL)
				fputs("__attribute__((visibility(\"hidden\")))\n", stream);
			transpile_c_fndec(modules, exp, 0, stream);
			fputs(";\n", stream);
		}
		else if (exp->ast_type == AST_VARDEC)
		{
			if (dict_find(&exp->attrs, &ext) != NULL)
				fputs("extern ", stream);
			else if (dict_find(&exp->attrs, &expo) == NULL)
				fputs("__attribute__((visibility(\"hidden\")))\n", stream);
			transpile_c_vardec(modules, exp, 0, stream);
			fputs(";\n", stream);
		}
	}
	return true;
}

bool transpile_c_code(struct ast* ast, FILE* stream, struct dict* modules)
{
	return transpile_c_ast(modules, ast, 0, stream);
}

static inline void transpile_c_newline_indent(FILE* stream, size_t indent)
{
	fputc('\n', stream);
	for (size_t i = 0; i < indent; ++i)
		fputc('\t', stream);
}

static inline bool transpile_c_ast(struct dict* modules, struct ast* ast, size_t indent, FILE* stream)
{
	switch (ast->ast_type)
	{
	case AST_ATTR: FALLTHROUGH;
	case AST_IMPDEC: FALLTHROUGH;
	case AST_MODDEC: FALLTHROUGH;
	case AST_ERROR: break;
	case AST_NUMLIT: fprintf(stream, "%" PRIu64, ast->value.numlit.u64); break;
	case AST_STRLIT:
		fputc('"', stream);
		cv_print(cv_str(&ast->value.strlit.str), stream);
		fputc('"', stream);
		break;
	case AST_VAR:
		if (ast->value.var.next != NULL) return transpile_c_ast(modules, ast->value.var.next, indent, stream);
		if (ast->value.var.dec == NULL) return transpile_c_builtin_var(cv_str(&ast->value.var.name), stream);
		struct cv cname = cv_cstr("cname");
		struct pair* pair = dict_find(&ast->value.var.dec->attrs, &cname);
		if (!pair)
			cv_print(cv_str(&ast->value.var.name), stream);
		else
			str_print(&(*LIST_GET(&(*PAIR_VAL(pair, struct ast*))->value.attr.args, struct ast*, 0))->value.strlit.str,
					  stream);
		break;
	case AST_UNARY:
		fputc((ast->value.unary.op == UNARY_PLUS ? '+' : '-'), stream);
		transpile_c_ast(modules, ast->value.unary.rhs, indent, stream);
		break;
	case AST_SEQ: {
		struct list* list = &ast->value.seq.list;
		for (size_t i = 0; i < list->size; ++i)
		{
			transpile_c_newline_indent(stream, indent);
			struct ast* exp = *LIST_GET(list, struct ast*, i);
			if (!transpile_c_ast(modules, exp, indent, stream)) return false;
		}
		break;
	}
	case AST_SEXP:
		transpile_c_ast(modules, ast->value.sexp.exp, indent, stream);
		fputc(';', stream);
		break;
	case AST_CALL:
		transpile_c_ast(modules, ast->value.call.name, indent, stream);
		fputc('(', stream);
		struct list* args = &ast->value.call.args;
		for (size_t i = 0; i < args->size; ++i)
		{
			if (i != 0) fputs(", ", stream);
			if (!transpile_c_ast(modules, *LIST_GET(args, struct ast*, i), indent, stream)) return false;
		}
		fputc(')', stream);
		break;
	case AST_VARDEC: {
		struct ast_vardec* vardec = &ast->value.vardec;
		transpile_c_vardec(modules, ast, indent, stream);
		fputs(" = ", stream);
		transpile_c_ast(modules, vardec->exp, indent, stream);
		fputc(';', stream);
		break;
	}
	case AST_FNDEC: {
		struct ast_fndec* fndec = &ast->value.fndec;
		transpile_c_fndec(modules, ast, indent, stream);
		if (fndec->exp == NULL)
			fputc(';', stream);
		else if (fndec->exp->ast_type == AST_SEQ)
		{
			fputs(" {", stream);
			transpile_c_ast(modules, fndec->exp, indent + 1, stream);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		else
		{
			fputs(" {", stream);
			transpile_c_newline_indent(stream, indent + 1);
			transpile_c_ast(modules, fndec->exp, indent + 1, stream);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		break;
	}
	case AST_RET:
		fputs("return ", stream);
		transpile_c_ast(modules, ast->value.ret.exp, indent, stream);
		fputc(';', stream);
		break;
	};
	return true;
}

static inline bool transpile_c_fndec(struct dict* modules, struct ast* ast, size_t indent, FILE* stream)
{
	struct ast_fndec* fndec = &ast->value.fndec;
	if (fndec->texp)
		transpile_c_ast(modules, fndec->texp, indent, stream);
	else
		fputs("void", stream);
	fputc(' ', stream);
	transpile_c_ast(modules, fndec->name, indent, stream);
	fputc('(', stream);
	if (fndec->args.size == 0) fputs("void", stream);
	for (size_t i = 0; i < fndec->args.size; ++i)
	{
		if (i != 0) fputs(", ", stream);
		struct ast* vardec = *LIST_GET(&fndec->args, struct ast*, i);
		if (!transpile_c_vardec(modules, vardec, indent, stream)) return false;
	}
	fputc(')', stream);
	return true;
}

static inline bool transpile_c_vardec(struct dict* modules, struct ast* ast, size_t indent, FILE* stream)
{
	struct ast_vardec* vardec = &ast->value.vardec;
	if (vardec->texp)
		transpile_c_ast(modules, vardec->texp, indent, stream);
	else
		fputs("int", stream);
	fputc(' ', stream);
	transpile_c_ast(modules, vardec->name, indent, stream);
	return true;
}

static inline bool transpile_c_builtin_var(struct cv name, FILE* stream)
{
	if (cv_cmp(name, cv_cstr("str")) == 0)
		fputs("char const*", stream);
	else
		cv_print(name, stream);
	return true;
}
