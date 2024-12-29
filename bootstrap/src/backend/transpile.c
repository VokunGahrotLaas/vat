#include "backend/transpile.h"

// libc
#include <sys/mman.h>
#include <sys/sendfile.h>

struct transpiler_c
{
	FILE* prelude;
	FILE* stream;
	FILE* header;
	bool error;
	bool first_seq;
	struct dict* modules;
	struct cv dest;
};

static inline void transpile_c_newline_indent(FILE* stream, size_t indent);
static inline bool transpile_c_ast(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_fndec(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_vardec(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream);
static inline bool sendfile_check(int fdout, int fdin, size_t size);
static inline bool transpile_c_builtin_var(struct cv name, FILE* stream);
static inline void add_headers(FILE* stream, struct module* mod);

bool transpile_c(struct ast* ast, char const* filename, struct dict* modules)
{
	FILE* stream = fopen(filename, "w");
	if (!stream)
	{
		fprintf(stderr, "could not open \"%s\" for writing\n", filename);
		return false;
	}
	struct str header_filename;
	str_of_cv(&header_filename, cv_cstr(filename));
	str_pushcv(&header_filename, cv_cstr(".h"));
	FILE* header = fopen(header_filename.data, "w");
	if (!header)
	{
		fclose(stream);
		fprintf(stderr, "could not open \"%s\" for writing\n", header_filename.data);
		return false;
	}
	str_dtor(&header_filename);
	for (size_t i = 0; i < modules->pairs.size; ++i)
	{
		struct pair* pair = LIST_GET(&modules->pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		add_headers(stream, *PAIR_VAL(pair, struct module*));
	}
	fputc('\n', stream);
	fputs("#pragma once\n#include <stdint.h>\n#include <stddef.h>\n#include <sys/types.h>\n\n", header);
	struct transpiler_c tp_c;
	tp_c.error = false;
	tp_c.first_seq = true;
	tp_c.prelude = stream;
	tp_c.stream = fdopen(memfd_create("transpiler_c", 0), "wr");
	tp_c.header = header;
	tp_c.modules = modules;
	tp_c.dest = cv_cstr(filename);
	bool r = transpile_c_ast(&tp_c, ast, 0, tp_c.stream);
	long size = ftell(tp_c.stream);
	fseek(tp_c.stream, 0, SEEK_SET);
	fflush(tp_c.prelude);
	fflush(stream);
	if (!sendfile_check(fileno(stream), fileno(tp_c.stream), size)) r = 1;
	fclose(tp_c.stream);
	fprintf(stream, "\n");
	fclose(stream);
	fclose(header);
	return r;
}

static inline void add_headers(FILE* stream, struct module* mod)
{
	if (mod->exports.size > 0) fprintf(stream, "#include \"%s.h\"\n\n", mod->compiled_file.data);
	for (size_t i = 0; i < mod->children.pairs.size; ++i)
	{
		struct pair* pair = LIST_GET(&mod->children.pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		add_headers(stream, *PAIR_VAL(pair, struct module*));
	}
}

static inline void transpile_c_newline_indent(FILE* stream, size_t indent)
{
	fputc('\n', stream);
	for (size_t i = 0; i < indent; ++i)
		fputc('\t', stream);
}

static inline bool transpile_c_ast(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream)
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
		if (ast->value.var.next != NULL) return transpile_c_ast(tp_c, ast->value.var.next, indent, stream);
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
		transpile_c_ast(tp_c, ast->value.unary.rhs, indent, stream);
		break;
	case AST_SEQ: {
		struct list* list = &ast->value.seq.list;
		if (tp_c->first_seq)
		{
			tp_c->first_seq = false;
			for (size_t i = 0; i < list->size; ++i)
			{
				struct ast* exp = *LIST_GET(list, struct ast*, i);
				struct cv expo = cv_cstr("export");
				struct cv ext = cv_cstr("extern");
				if (exp->ast_type == AST_FNDEC)
				{
					if (dict_find(&exp->attrs, &expo) != NULL)
					{
						fputs("extern ", tp_c->header);
						transpile_c_fndec(tp_c, exp, 0, tp_c->header);
						fputs(";\n", tp_c->header);
					}
					else if (dict_find(&exp->attrs, &ext) == NULL)
						fputs("__attribute__((visibility(\"hidden\")))\n", tp_c->prelude);
					transpile_c_fndec(tp_c, exp, 0, tp_c->prelude);
					fputs(";\n", tp_c->prelude);
				}
				else if (exp->ast_type == AST_VARDEC)
				{
					if (dict_find(&exp->attrs, &expo) != NULL)
					{
						fputs("extern ", tp_c->header);
						transpile_c_vardec(tp_c, exp, 0, tp_c->header);
						fputs(";\n", tp_c->header);
					}
					else if (dict_find(&exp->attrs, &ext) == NULL)
						fputs("__attribute__((visibility(\"hidden\")))\n", tp_c->prelude);
					transpile_c_vardec(tp_c, exp, 0, tp_c->prelude);
					fputs(";\n", tp_c->prelude);
				}
			}
		}
		for (size_t i = 0; i < list->size; ++i)
		{
			transpile_c_newline_indent(stream, indent);
			struct ast* exp = *LIST_GET(list, struct ast*, i);
			if (!transpile_c_ast(tp_c, exp, indent, stream)) return false;
		}
		break;
	}
	case AST_SEXP:
		transpile_c_ast(tp_c, ast->value.sexp.exp, indent, stream);
		fputc(';', stream);
		break;
	case AST_CALL:
		transpile_c_ast(tp_c, ast->value.call.name, indent, stream);
		fputc('(', stream);
		struct list* args = &ast->value.call.args;
		for (size_t i = 0; i < args->size; ++i)
		{
			if (i != 0) fputs(", ", stream);
			if (!transpile_c_ast(tp_c, *LIST_GET(args, struct ast*, i), indent, stream)) return false;
		}
		fputc(')', stream);
		break;
	case AST_VARDEC: {
		struct ast_vardec* vardec = &ast->value.vardec;
		transpile_c_vardec(tp_c, ast, indent, stream);
		fputs(" = ", stream);
		transpile_c_ast(tp_c, vardec->exp, indent, stream);
		fputc(';', stream);
		break;
	}
	case AST_FNDEC: {
		struct ast_fndec* fndec = &ast->value.fndec;
		transpile_c_fndec(tp_c, ast, indent, stream);
		if (fndec->exp == NULL)
			fputc(';', stream);
		else if (fndec->exp->ast_type == AST_SEQ)
		{
			fputs(" {", stream);
			transpile_c_ast(tp_c, fndec->exp, indent + 1, stream);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		else
		{
			fputs(" {", stream);
			transpile_c_newline_indent(stream, indent + 1);
			transpile_c_ast(tp_c, fndec->exp, indent + 1, stream);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		break;
	}
	case AST_RET:
		fputs("return ", stream);
		transpile_c_ast(tp_c, ast->value.ret.exp, indent, stream);
		fputc(';', stream);
		break;
	};
	return true;
}

static inline bool transpile_c_fndec(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream)
{
	struct ast_fndec* fndec = &ast->value.fndec;
	struct cv ext = cv_cstr("extern");
	if (dict_find(&ast->attrs, &ext) != NULL && tp_c->header != stream) fputs("extern ", stream);
	if (fndec->texp)
		transpile_c_ast(tp_c, fndec->texp, indent, stream);
	else
		fputs("void", stream);
	fputc(' ', stream);
	transpile_c_ast(tp_c, fndec->name, indent, stream);
	fputc('(', stream);
	if (fndec->args.size == 0) fputs("void", stream);
	for (size_t i = 0; i < fndec->args.size; ++i)
	{
		if (i != 0) fputs(", ", stream);
		struct ast* vardec = *LIST_GET(&fndec->args, struct ast*, i);
		if (!transpile_c_vardec(tp_c, vardec, indent, stream)) return false;
	}
	fputc(')', stream);
	return true;
}

static inline bool transpile_c_vardec(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream)
{
	struct ast_vardec* vardec = &ast->value.vardec;
	if (vardec->texp)
		transpile_c_ast(tp_c, vardec->texp, indent, stream);
	else
		fputs("int", stream);
	fputc(' ', stream);
	transpile_c_ast(tp_c, vardec->name, indent, stream);
	return true;
}

static inline bool sendfile_check(int fdout, int fdin, size_t size)
{
	ssize_t nsent = 0;
	while (nsent != -1 && (size_t)nsent < size)
		nsent += sendfile(fdout, fdin, NULL, size - nsent);
	if (nsent == -1) warn("sendfile() failed");
	return nsent != -1;
}

static inline bool transpile_c_builtin_var(struct cv name, FILE* stream)
{
	if (cv_cmp(name, cv_cstr("str")) == 0)
		fputs("char const*", stream);
	else
		cv_print(name, stream);
	return true;
}
