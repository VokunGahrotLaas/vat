#include "transpile/transpile.h"

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
	fprintf(stream, "#include <stddef.h>\n");
	fprintf(stream, "\n");
	fprintf(stream, "int main(void) {");
	bool r = transpile_c_ast(ast, stream, 1);
	fprintf(stream, "\n");
	fprintf(stream, "\treturn 0;\n");
	fprintf(stream, "}\n");
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
	switch (ast->type)
	{
	case AST_ERROR: UNREACHABLE();
	case AST_NUMBER: fprintf(stream, "%" PRIu64, ast->value.number.u64); break;
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
			if (!transpile_c_ast(*LIST_GET(list, struct ast*, i), stream, indent)) return false;
			fputc(';', stream);
		}
		break;
	}
	};
	return true;
}
