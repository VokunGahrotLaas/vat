#include "backend/transpile.h"

// libc
#include <sys/mman.h>
#include <sys/sendfile.h>

struct transpiler_c
{
	FILE* prelude;
	FILE* stream;
	bool error;
	bool first_seq;
};

static inline void transpile_c_newline_indent(FILE* stream, size_t indent);
static inline bool transpile_c_ast(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_fndec(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream);
static inline bool transpile_c_vardec(struct transpiler_c* tp_c, struct ast* ast, size_t indent, FILE* stream);
static inline bool sendfile_check(int fdout, int fdin, size_t size);

bool transpile_c(struct ast* ast, char const* filename)
{
	FILE* stream = fopen(filename, "w");
	if (!stream)
	{
		fprintf(stderr, "could not open \"%s\" for writing\n", filename);
		return false;
	}
	fprintf(stream, "#include <stdio.h>\n\n");
	struct transpiler_c tp_c;
	tp_c.error = false;
	tp_c.first_seq = true;
	tp_c.prelude = stream;
	tp_c.stream = fdopen(memfd_create("transpiler_c", 0), "wr");
	bool r = transpile_c_ast(&tp_c, ast, 0, tp_c.stream);
	long size = ftell(tp_c.stream);
	fseek(tp_c.stream, 0, SEEK_SET);
	fflush(tp_c.prelude);
	fflush(stream);
	if (!sendfile_check(fileno(stream), fileno(tp_c.stream), size)) r = 1;
	fclose(tp_c.stream);
	fprintf(stream, "\n");
	fclose(stream);
	return r;
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
	case AST_ERROR: UNREACHABLE();
	case AST_NUMLIT: fprintf(stream, "%" PRIu64, ast->value.numlit.u64); break;
	case AST_STRLIT:
		fputc('"', stream);
		cv_print(cv_str(&ast->value.word.str), stream);
		fputc('"', stream);
		break;
	case AST_WORD: cv_print(cv_str(&ast->value.word.str), stream); break;
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
				if (exp->ast_type == AST_FNDEC)
				{
					transpile_c_fndec(tp_c, exp, 0, tp_c->prelude);
					fputs(";\n", tp_c->prelude);
				}
				else if (exp->ast_type == AST_VARDEC)
				{
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
		transpile_c_ast(tp_c, ast->value.call.fun, indent, stream);
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
		fputc(' ', stream);
		if (fndec->exp->ast_type == AST_SEQ)
		{
			fputc('{', stream);
			transpile_c_ast(tp_c, fndec->exp, indent + 1, stream);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		else
		{
			fputc('{', stream);
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
		if (!transpile_c_ast(tp_c, *LIST_GET(&fndec->args, struct ast*, i), indent, stream)) return false;
		struct ast* texp = *LIST_GET(&fndec->targs, struct ast*, i);
		if (!texp) continue;
		fputs(": ", stream);
		if (!transpile_c_ast(tp_c, texp, indent, stream)) return false;
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
