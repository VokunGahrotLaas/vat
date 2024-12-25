#include "backend/transpile.h"

static inline void transpile_c_newline_indent(FILE* stream, size_t indent);

static inline bool transpile_c_ast(struct ast* ast, FILE* stream, size_t indent);

bool transpile_c(struct ast* ast, char const* filename)
{
	FILE* stream = fopen(filename, "w");
	if (!stream)
	{
		fprintf(stderr, "could not open \"%s\" for writing\n", filename);
		return false;
	}
	fprintf(stream, "#include <stdio.h>\n");
	bool r = transpile_c_ast(ast, stream, 0);
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

static inline bool transpile_c_ast(struct ast* ast, FILE* stream, size_t indent)
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
		transpile_c_ast(ast->value.unary.rhs, stream, indent);
		break;
	case AST_SEQ: {
		struct list* list = &ast->value.seq.list;
		for (size_t i = 0; i < list->size; ++i)
		{
			transpile_c_newline_indent(stream, indent);
			struct ast* exp = *LIST_GET(list, struct ast*, i);
			if (!transpile_c_ast(exp, stream, indent)) return false;
		}
		break;
	}
	case AST_SEXP:
		transpile_c_ast(ast->value.sexp.exp, stream, indent);
		fputc(';', stream);
		break;
	case AST_CALL:
		transpile_c_ast(ast->value.call.fun, stream, indent);
		fputc('(', stream);
		struct list* args = &ast->value.call.args;
		for (size_t i = 0; i < args->size; ++i)
		{
			if (i != 0) fputs(", ", stream);
			if (!transpile_c_ast(*LIST_GET(args, struct ast*, i), stream, indent)) return false;
		}
		fputc(')', stream);
		break;
	case AST_VARDEC: {
		struct ast_vardec* vardec = &ast->value.vardec;
		if (vardec->texp)
			transpile_c_ast(vardec->texp, stream, indent);
		else
			fputs("int", stream);
		fputc(' ', stream);
		transpile_c_ast(vardec->name, stream, indent);
		fputs(" = ", stream);
		transpile_c_ast(vardec->exp, stream, indent);
		fputc(';', stream);
		break;
	}
	case AST_FNDEC: {
		struct ast_fndec* fndec = &ast->value.fndec;
		if (fndec->texp)
			transpile_c_ast(fndec->texp, stream, indent);
		else
			fputs("void", stream);
		fputc(' ', stream);
		transpile_c_ast(fndec->name, stream, indent);
		fputc('(', stream);
		if (fndec->args.size == 0) fputs("void", stream);
		for (size_t i = 0; i < fndec->args.size; ++i)
		{
			if (i != 0) fputs(", ", stream);
			if (!transpile_c_ast(*LIST_GET(&fndec->args, struct ast*, i), stream, indent)) return false;
			struct ast* texp = *LIST_GET(&fndec->targs, struct ast*, i);
			if (!texp) continue;
			fputs(": ", stream);
			if (!transpile_c_ast(texp, stream, indent)) return false;
		}
		fputs(") ", stream);
		if (fndec->exp->ast_type == AST_SEQ)
		{
			fputc('{', stream);
			transpile_c_ast(fndec->exp, stream, indent + 1);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		else
		{
			fputc('{', stream);
			transpile_c_newline_indent(stream, indent + 1);
			transpile_c_ast(fndec->exp, stream, indent + 1);
			transpile_c_newline_indent(stream, indent);
			fputc('}', stream);
		}
		break;
	}
	case AST_RET:
		fputs("return ", stream);
		transpile_c_ast(ast->value.ret.exp, stream, indent);
		fputc(';', stream);
		break;
	};
	return true;
}
