// libc
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
// bootstrap
#include "compile/compile.h"
#include "parser/parser.h"
#include "transpile/transpile.h"

enum main_state
{
	MAIN_NONE = 0,
	MAIN_ERROR,
	MAIN_HELP,
	MAIN_PRINT_TOKENS,
	MAIN_PRINT_AST,
	MAIN_TRANSPILE,
	MAIN_COMPILE,
};

int main_help(char const* name, FILE* stream, int r);
int main_lexer(char const* source);
int main_parser(char const* source);
int main_transpile_c(char const* source, char const* dest);
int main_compile_c(char const* source, char const* dest);

int main(int argc, char** argv)
{
	struct str output_str;
	str_ctor(&output_str, 0);
	char const* output = NULL;
	char const* source = NULL;
	enum main_state state = MAIN_NONE;

	struct option l_opt[] = {
		{ "transpile",	   no_argument,		NULL, 't' },
		{ "compile",		 no_argument,		  NULL, 'c' },
		{ "print-ast",	   no_argument,		NULL, 'A' },
		{ "print-tokens", no_argument,	   NULL, 'T' },
		{ "help",		  no_argument,	   NULL, 'h' },
		{ "output",		required_argument, NULL, 'o' },
		{ NULL,			0,				   NULL, 0   },
	};
	char const* s_opt = "tco:";
	int c_opt = -1;
	while ((c_opt = getopt_long(argc, argv, s_opt, l_opt, NULL)) != -1)
	{
		if (c_opt == 'o')
		{
			if (output != NULL)
			{
				state = MAIN_ERROR;
				warnx("-o can only but passed once");
			}
			else
				output = optarg;
		}
		else if (c_opt == 'c')
		{
			if (state != MAIN_NONE && state != MAIN_COMPILE)
			{
				state = MAIN_ERROR;
				warnx("-c cannot be combined with other actions");
			}
			else
				state = MAIN_COMPILE;
		}
		else if (c_opt == 't')
		{
			if (state != MAIN_NONE && state != MAIN_TRANSPILE)
			{
				state = MAIN_ERROR;
				warnx("-t cannot be combined with other actions");
			}
			else
				state = MAIN_TRANSPILE;
		}
		else if (c_opt == 'A')
		{
			if (state != MAIN_NONE && state != MAIN_PRINT_AST)
			{
				state = MAIN_ERROR;
				warnx("-A cannot be combined with other actions");
			}
			else
				state = MAIN_PRINT_AST;
		}
		else if (c_opt == 'T')
		{
			if (state != MAIN_NONE && state != MAIN_PRINT_TOKENS)
			{
				state = MAIN_ERROR;
				warnx("-T cannot be combined with other actions");
			}
			else
				state = MAIN_PRINT_TOKENS;
		}
		else if (c_opt == 'h')
		{
			if (state != MAIN_NONE && state != MAIN_HELP)
			{
				state = MAIN_ERROR;
				warnx("-h cannot be combined with other actions");
			}
			else
				state = MAIN_HELP;
		}
		else
		{
			state = MAIN_ERROR;
			warnx("unknown option \"%s\"", argv[optind]);
		}
	}

	if (state != MAIN_HELP && optind + 1 != argc && state != MAIN_ERROR)
	{
		state = MAIN_ERROR;
		if (optind + 1 < argc)
			warnx("missing source file");
		else
			warnx("too many source files");
	}
	if (!(state == MAIN_NONE || state == MAIN_COMPILE || state == MAIN_TRANSPILE) && output && state != MAIN_ERROR)
	{
		state = MAIN_ERROR;
		warnx("this action does not support -o");
	}

	if (state != MAIN_HELP && optind + 1 == argc && state != MAIN_ERROR) source = argv[optind];
	if ((state == MAIN_NONE || state == MAIN_COMPILE || state == MAIN_TRANSPILE) && !output && source
		&& state != MAIN_ERROR)
	{
		char* src = basename(source);
		size_t len = strlen(src);
		str_ctor(&output_str, len);
		bool has_dot_vat = len > 4 && strcmp(src + len - 4, ".vat") == 0;
		str_pushcv(&output_str, (struct cv){ src, len - (has_dot_vat ? 4 : 0) });
		str_pushcv(&output_str, cv_cstr(state == MAIN_TRANSPILE ? ".c" : ".out"));
		output = output_str.data;
	}

	int r = 1;
	switch (state)
	{
	case MAIN_ERROR: r = main_help(*argv, stderr, 1); break;
	case MAIN_HELP: r = main_help(*argv, stdout, 0); break;
	case MAIN_NONE: FALLTHROUGH;
	case MAIN_COMPILE: r = main_compile_c(source, output); break;
	case MAIN_TRANSPILE: r = main_transpile_c(source, output); break;
	case MAIN_PRINT_AST: r = main_parser(source); break;
	case MAIN_PRINT_TOKENS: r = main_lexer(source); break;
	};

	str_dtor(&output_str);
	return r;
}

int main_help(char const* name, FILE* stream, int r)
{
	fprintf(stream, "USAGE: %s [OPTIONS] <filename.vat>\n", name);
	fprintf(stream, "\n");
	fprintf(stream, "OPTIONS:\n");
	fprintf(stream, "  -o/--output <file>: specify output file\n");
	fprintf(stream, "  -h/--help: prints this message\n");
	return r;
}

int main_lexer(char const* source)
{
	struct lexer lexer;
	lexer_of_file(&lexer, source);
	bool eof = false;
	while (!eof)
	{
		if (lexer_peek(&lexer).type == TOKEN_EOF) eof = true;
		struct token token = lexer_pop(&lexer);
		loc_print(&token.loc, stdout);
		fputs(": ", stdout);
		token_print(&token, stdout);
		fputc('\n', stdout);
		token_dtor(&token);
	}
	int r = lexer.error ? 1 : 0;
	lexer_dtor(&lexer);
	return r;
}

int main_parser(char const* source)
{
	struct parser parser;
	parser_of_file(&parser, source);
	struct ast* ast = parser_parse(&parser);
	int r = parser.lexer.error || parser.error ? 1 : 0;
	parser_dtor(&parser);
	ast_print(ast, stdout);
	ast_free(ast);
	return r;
}

int main_transpile_c(char const* source, char const* dest)
{
	struct parser parser;
	parser_of_file(&parser, source);
	struct ast* ast = parser_parse(&parser);
	int r = parser.lexer.error || parser.error ? 1 : 0;
	parser_dtor(&parser);
	if (r) return r;
	r = !transpile_c(ast, dest) ? 1 : 0;
	ast_free(ast);
	return r;
}

int main_compile_c(char const* source, char const* dest)
{
	struct parser parser;
	parser_of_file(&parser, source);
	struct ast* ast = parser_parse(&parser);
	int r = parser.lexer.error || parser.error ? 1 : 0;
	parser_dtor(&parser);
	if (r) return r;
	char tmp_file[] = "/tmp/vatc-transpile_c-XXXXXX.c";
	if (!mkstemps(tmp_file, 2))
	{
		ast_free(ast);
		return 1;
	}
	r = !transpile_c(ast, tmp_file) ? 1 : 0;
	ast_free(ast);
	if (r)
	{
		unlink(tmp_file);
		return r;
	}
	r = !compile_c(tmp_file, dest) ? 1 : 0;
	unlink(tmp_file);
	return r;
}
