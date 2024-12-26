// libc
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
// bootstrap
#include "backend/compile.h"
#include "backend/transpile.h"
#include "parser/parser.h"
#include "utils/dict.h"

enum main_state
{
	MAIN_NONE = 0,
	MAIN_ERROR,
	MAIN_HELP,
	MAIN_PRINT_TOKENS,
	MAIN_PRINT_AST,
	MAIN_TRANSPILE,
	MAIN_COMPILE,
	MAIN_RUN,
	MAIN_CHECK,
};

enum backend
{
	BACKEND_NONE = 0,
	BACKEND_C,
};

typedef bool backend_transpile_t(struct ast* ast, char const* dest);
typedef bool backend_compile_t(char const* source, char const* dest);

struct vbackend
{
	backend_transpile_t* transpile;
	backend_compile_t* compile;
};

int main_help(char const* name, FILE* stream, int r);
int main_lexer(char const* source);
int main_parser(char const* source);
int main_transpile_c(char const* source, char const* dest, struct vbackend* backend);
int main_compile_c(char const* source, char const* dest, struct vbackend* backend);
int main_run_c(char const* source, struct vbackend* backend);
int main_check(char const* source, struct vbackend* backend);

int main(int argc, char** argv)
{
	struct str output_str;
	str_ctor(&output_str, 0);
	char const* output = NULL;
	char const* source = NULL;
	enum main_state state = MAIN_NONE;
	enum backend backend = BACKEND_NONE;
	struct vbackend vbackend[] = {
		[BACKEND_NONE] = {
			.transpile = &transpile_c,
			.compile = &compile_c,
		},
		[BACKEND_C] = {
			.transpile = &transpile_c,
			.compile = &compile_c,
		},
	};

	struct option l_opt[] = {
		{ "transpile",	   no_argument,		NULL, 't' },
		 { "compile",	  no_argument,	   NULL, 'c' },
		{ "run",			 no_argument,		  NULL, 'r' },
		 { "print-ast",	no_argument,		 NULL, 'A' },
		{ "print-tokens", no_argument,	   NULL, 'T' },
		 { "check",		no_argument,		 NULL, 'C' },
		{ "help",		  no_argument,	   NULL, 'h' },
		 { "output",		 required_argument, NULL, 'o' },
		{ "backend",		 required_argument, NULL, 'b' },
		 { NULL,			 0,					NULL, 0	},
	};
	char const* s_opt = "tcrATCho:b:";
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
		else if (c_opt == 'b')
		{
			if (backend != BACKEND_NONE)
			{
				state = MAIN_ERROR;
				warnx("-b can only but passed once");
			}
			else if (strcmp(optarg, "C") == 0 || strcmp(optarg, "c") == 0)
				backend = BACKEND_C;
			else
			{
				state = MAIN_ERROR;
				warnx("invalid backend \"%s\"", optarg);
			}
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
		else if (c_opt == 'r')
		{
			if (state != MAIN_NONE && state != MAIN_RUN)
			{
				state = MAIN_ERROR;
				warnx("-r cannot be combined with other actions");
			}
			else
				state = MAIN_RUN;
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
		else if (c_opt == 'C')
		{
			if (state != MAIN_NONE && state != MAIN_CHECK)
			{
				state = MAIN_ERROR;
				warnx("-C cannot be combined with other actions");
			}
			else
				state = MAIN_CHECK;
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
		if (optind == argc)
			warnx("missing source file");
		else
			warnx("too many source files (%i)", argc - optind - 1);
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
	case MAIN_COMPILE: r = main_compile_c(source, output, &vbackend[backend]); break;
	case MAIN_TRANSPILE: r = main_transpile_c(source, output, &vbackend[backend]); break;
	case MAIN_RUN: r = main_run_c(source, &vbackend[backend]); break;
	case MAIN_PRINT_AST: r = main_parser(source); break;
	case MAIN_PRINT_TOKENS: r = main_lexer(source); break;
	case MAIN_CHECK: r = main_check(source, &vbackend[backend]); break;
	};

	str_dtor(&output_str);
	return r;
}

int main_help(char const* name, FILE* stream, int r)
{
	fprintf(stream, "USAGE: %s [OPTIONS] <source-file.vat>\n", name);
	fprintf(stream, "\n");
	fprintf(stream, "OPTIONS:\n");
	fprintf(stream, "  -o/--output <file>: specify output file\n");
	fprintf(stream, "  -b/--backend C: specify backend type (only C for now)\n");
	fprintf(stream, "  -t/--transpile: transpile source to backend\n");
	fprintf(stream, "  -c/--compile: compile source to executable with backend (default)\n");
	fprintf(stream, "  -r/--run: run compiled code\n");
	fprintf(stream, "  -T/--print-tokens: print all the tokens from the source\n");
	fprintf(stream, "  -T/--print-ast: print the AST from the source (displayed as source code)\n");
	fprintf(stream, "  -h/--help: print this message\n");
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

int main_transpile_c(char const* source, char const* dest, struct vbackend* backend)
{
	struct parser parser;
	parser_of_file(&parser, source);
	struct ast* ast = parser_parse(&parser);
	int r = parser.lexer.error || parser.error ? 1 : 0;
	parser_dtor(&parser);
	if (r) return r;
	r = !(*backend->transpile)(ast, dest) ? 1 : 0;
	ast_free(ast);
	return r;
}

int main_compile_c(char const* source, char const* dest, struct vbackend* backend)
{
	char tmp_file[] = "/tmp/vatc-transpile_c-XXXXXX.c";
	if (!mkstemps(tmp_file, 2)) return 1;
	int r = main_transpile_c(source, tmp_file, backend);
	if (r)
	{
		unlink(tmp_file);
		return r;
	}
	r = !(*backend->compile)(tmp_file, dest) ? 1 : 0;
	unlink(tmp_file);
	return r;
}

int main_run_c(char const* source, struct vbackend* backend)
{
	char tmp_file[] = "/tmp/vatc-run_c-XXXXXX.out";
	if (!mkstemps(tmp_file, 4)) return 1;
	int r = main_compile_c(source, tmp_file, backend);
	if (r)
	{
		unlink(tmp_file);
		return r;
	}
	r = !run(tmp_file) ? 1 : 0;
	unlink(tmp_file);
	return r;
}

int main_check(UNUSED char const* source, UNUSED struct vbackend* backend)
{
	VTYPE(vtype_int, int, NULL, NULL, NULL, NULL, NULL);
	VPAIR(vpair_int_int, &vtype_int, &vtype_int);
	VDICT(vdict_int_int, int, int, &vpair_int_int);

	struct dict dict;
	dict_ctor(&dict, &vdict_int_int, 32);

	for (int k = 0; k < 10; ++k)
	{
		int v = k * k;
		struct pair* pair = dict_add_copy(&dict, &k, &v);
		if (pair != NULL)
			printf("add(%i, %i) => {%i, %i}\n", k, v, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("add(%i, %i) => failure\n", k, v);
	}

	for (int k = -1; k < 16; ++k)
	{
		struct pair* pair = dict_find(&dict, &k);
		if (pair != NULL)
			printf("find(%i) => {%i, %i}\n", k, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("find(%i) => none\n", k);
	}

	for (int k = 4; k < 9; ++k)
	{
		bool r = dict_remove(&dict, &k);
		printf("remove(%i) => %s\n", k, (r ? "success" : "failure"));
	}

	for (int k = -1; k < 16; ++k)
	{
		struct pair* pair = dict_find(&dict, &k);
		if (pair != NULL)
			printf("find(%i) => {%i, %i}\n", k, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("find(%i) => none\n", k);
	}

	dict_dtor(&dict);
	return 0;
}
