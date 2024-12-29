// libc
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
// bootstrap
#include "backend/compile.h"
#include "backend/transpile.h"
#include "parser/parser.h"
#include "post/binder.h"
#include "post/module_binder.h"
#include "utils/sdict.h"

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

typedef bool backend_transpile_t(struct ast* ast, char const* dest, struct dict* modules);
typedef bool backend_compile_t(struct list const* sources, char const* dest);

struct vbackend
{
	struct dict modules;
	backend_transpile_t* transpile;
	backend_compile_t* compile;
};

VLIST(vlist_cv, struct cv, &vtype_cv);

static inline int main_help(char const* name, FILE* stream, int r);
static inline int main_lexer(char const** sources);
static inline int main_parser(char const** sources);
static inline int main_transpile_c(char const** sources, char const* dest, struct vbackend* backend);
static inline int main_compile_c(char const** sources, char const* dest, struct vbackend* backend);
static inline int main_run_c(char const** sources, struct vbackend* backend);
static inline int main_check(char const** sources);
static inline int main_check_dict(void);

static inline int main_check_sdict(void);

int main(int argc, char** argv)
{
	struct str output_str;
	str_ctor(&output_str, 0);
	char const* output = NULL;
	char const** sources = NULL;
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

	if (state != MAIN_HELP && state != MAIN_ERROR)
	{
		if (optind + 1 > argc)
		{
			state = MAIN_ERROR;
			warnx("missing source file");
		}
		sources = (char const**)&argv[optind];
		optind = argc;
	}
	if (!(state == MAIN_NONE || state == MAIN_COMPILE || state == MAIN_TRANSPILE) && output && state != MAIN_ERROR)
	{
		state = MAIN_ERROR;
		warnx("this action does not support -o");
	}
	if (optind < argc && state != MAIN_ERROR)
	{
		state = MAIN_ERROR;
		warnx("too many arguments (%i)", argc - optind);
	}

	if ((state == MAIN_NONE || state == MAIN_COMPILE || state == MAIN_TRANSPILE) && !output && sources
		&& state != MAIN_ERROR)
	{
		char* src = basename(*sources);
		size_t len = strlen(src);
		str_ctor(&output_str, len);
		bool has_dot_vat = len > 4 && strcmp(src + len - 4, ".vat") == 0;
		str_pushcv(&output_str, (struct cv){ src, len - (has_dot_vat ? 4 : 0) });
		str_pushcv(&output_str, cv_cstr(state == MAIN_TRANSPILE ? ".c" : ".out"));
		output = output_str.data;
	}

	dict_ctor(&vbackend[backend].modules, &vdict_cv_upmodule, 0);

	int r = 1;
	switch (state)
	{
	case MAIN_ERROR: r = main_help(*argv, stderr, 1); break;
	case MAIN_HELP: r = main_help(*argv, stdout, 0); break;
	case MAIN_NONE: FALLTHROUGH;
	case MAIN_COMPILE: r = main_compile_c(sources, output, &vbackend[backend]); break;
	case MAIN_TRANSPILE: r = main_transpile_c(sources, output, &vbackend[backend]); break;
	case MAIN_RUN: r = main_run_c(sources, &vbackend[backend]); break;
	case MAIN_PRINT_AST: r = main_parser(sources); break;
	case MAIN_PRINT_TOKENS: r = main_lexer(sources); break;
	case MAIN_CHECK: r = main_check(sources); break;
	};

	dict_dtor(&vbackend[backend].modules);
	str_dtor(&output_str);
	return r;
}

static inline int main_help(char const* name, FILE* stream, int r)
{
	fprintf(stream, "USAGE: %s [OPTIONS] <source1.vat> [<source2.vat> ...]\n", name);
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

static inline int main_lexer(char const** sources)
{
	int r = 0;
	for (char const** source = sources; *source; ++source)
	{
		fprintf(stderr, "lexing: %s\n", *source);
		struct lexer lexer;
		lexer_of_file(&lexer, *source);
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
		r = r == 1 || lexer.error ? 1 : 0;
		lexer_dtor(&lexer);
	}
	return r;
}

static inline int main_parser(char const** sources)
{
	int r = 0;
	struct dict modules;
	dict_ctor(&modules, &vdict_cv_upmodule, 16);

	for (char const** source = sources; *source; ++source)
	{
		fprintf(stderr, "parsing: %s\n", *source);
		struct parser parser;
		parser_of_file(&parser, *source);
		struct ast* ast = parser_parse(&parser);
		if (parser.lexer.error || parser.error) r = 1;
		parser_dtor(&parser);

		struct module_binder mbinder;
		module_binder_ctor(&mbinder, &modules, cv_cstr(*source));
		module_binder_bind(&mbinder, ast);
		struct module* module = mbinder.module;
		if (mbinder.error) r = 1;
		module_binder_dtor(&mbinder);

		fputs("module_name: ", stderr);
		module_print(module, stderr);
		fputc('\n', stderr);
		fputs("exports_names: ", stderr);
		dict_print(&module->exports_names, stderr);
		fputc('\n', stderr);
		fputs("exports: ", stderr);
		dict_print(&module->exports, stderr);
		fputc('\n', stderr);

		ast_free(ast);
	}

	for (char const** source = sources; *source; ++source)
	{
		fprintf(stderr, "parsing: %s\n", *source);
		struct parser parser;
		parser_of_file(&parser, *source);
		struct ast* ast = parser_parse(&parser);
		if (parser.lexer.error || parser.error) r = 1;
		parser_dtor(&parser);

		struct binder binder;
		binder_ctor(&binder, &modules);
		binder_bind(&binder, ast);
		if (binder.error) r = 1;
		binder_dtor(&binder);

		ast_print(ast, stdout);
		ast_free(ast);
	}

	dict_dtor(&modules);
	return r;
}

static inline int main_transpile_c(char const** sources, char const* dest, struct vbackend* backend)
{
	int r = 0;
	dict_ctor(&backend->modules, &vdict_cv_upmodule, 16);

	struct list dests;
	list_ctor(&dests, &vlist_cv, 16);

	for (char const** source = sources; *source; ++source)
	{
		fprintf(stderr, "parsing module from: %s\n", *source);
		struct parser parser;
		parser_of_file(&parser, *source);
		struct ast* ast = parser_parse(&parser);
		if (parser.lexer.error || parser.error) r = 1;
		parser_dtor(&parser);

		struct module_binder mbinder;
		module_binder_ctor(&mbinder, &backend->modules, cv_cstr(*source));
		module_binder_bind(&mbinder, ast);
		struct module* module = mbinder.module;
		if (mbinder.error) r = 1;
		module_binder_dtor(&mbinder);

		char tmp_file[] = "/tmp/vatc-transpile_c-XXXXXX.c";
		if (source != sources)
		{
			if (!mkstemps(tmp_file, 2)) return 1;
			str_pushcv(&module->compiled_file, cv_cstr(tmp_file));
		}
		else
			str_pushcv(&module->compiled_file, cv_cstr(dest));
		struct cv dst = cv_str(&module->compiled_file);
		list_push_move(&dests, &dst);

		fputs("module_name: ", stderr);
		module_print(module, stderr);
		fputc('\n', stderr);
		fputs("exports_names: ", stderr);
		dict_print(&module->exports_names, stderr);
		fputc('\n', stderr);
		fputs("exports: ", stderr);
		dict_print(&module->exports, stderr);
		fputc('\n', stderr);

		ast_free(ast);
	}

	size_t i = 0;
	for (char const** source = sources; *source; ++source, ++i)
	{
		fprintf(stderr, "binding code from: %s\n", *source);
		struct parser parser;
		parser_of_file(&parser, *source);
		struct ast* ast = parser_parse(&parser);
		if (parser.lexer.error || parser.error) r = 1;
		parser_dtor(&parser);

		struct binder binder;
		binder_ctor(&binder, &backend->modules);
		binder_bind(&binder, ast);
		if (binder.error) r = 1;
		binder_dtor(&binder);

		if (!(*backend->transpile)(ast, LIST_GET(&dests, struct cv, i)->data, &backend->modules)) r = 1;
		ast_free(ast);
	}

	list_dtor(&dests);

	return r;
}

static inline void add_sources(struct list* srcs, struct module* mod);

static inline int main_compile_c(char const** sources, char const* dest, struct vbackend* backend)
{
	char tmp_file[] = "/tmp/vatc-transpile_c-XXXXXX.c";
	if (!mkstemps(tmp_file, 2)) return 1;
	int r = main_transpile_c(sources, tmp_file, backend);
	if (r)
	{
		unlink(tmp_file);
		return r;
	}
	struct list srcs;
	list_ctor(&srcs, &vlist_pchar, 16);
	for (size_t i = 0; i < backend->modules.pairs.size; ++i)
	{
		struct pair* pair = LIST_GET(&backend->modules.pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		struct module* mod = *PAIR_VAL(pair, struct module*);
		add_sources(&srcs, mod);
	}
	r = !(*backend->compile)(&srcs, dest) ? 1 : 0;
	list_dtor(&srcs);
	unlink(tmp_file);
	return r;
}

static inline void add_sources(struct list* srcs, struct module* mod)
{
	if (mod->compiled_file.size > 0) list_push_copy(srcs, &mod->compiled_file.data);
	for (size_t i = 0; i < mod->children.pairs.size; ++i)
	{
		struct pair* pair = LIST_GET(&mod->children.pairs, struct pair, i);
		if (pair_status(pair) != PAIR_SET) continue;
		add_sources(srcs, *PAIR_VAL(pair, struct module*));
	}
}

static inline int main_run_c(char const** sources, struct vbackend* backend)
{
	char tmp_file[] = "/tmp/vatc-run_c-XXXXXX.out";
	if (!mkstemps(tmp_file, 4)) return 1;
	int r = main_compile_c(sources, tmp_file, backend);
	if (r)
	{
		unlink(tmp_file);
		return r;
	}
	r = !run(tmp_file) ? 1 : 0;
	unlink(tmp_file);
	return r;
}

static inline int main_check(char const** sources)
{
	int r = 0;
	for (char const** source = sources; *source; ++source)
	{
		if (strcmp(*source, "dict") == 0)
		{
			int sr = main_check_dict();
			if (sr != 0) r = sr;
		}
		else if (strcmp(*source, "sdict") == 0)
		{
			int sr = main_check_sdict();
			if (sr != 0) r = sr;
		}
		else
		{
			warnx("main_check: invalid name \"%s\"", *source);
			r = 1;
		}
	}
	return r;
}

void int_print(int const* i, FILE* stream);

VTYPE(vtype_int, int, NULL, NULL, NULL, NULL, NULL, &int_print);
VPAIR(vpair_int_int, &vtype_int, &vtype_int);
VDICT(vdict_int_int, int, int, &vpair_int_int);

static inline int main_check_dict(void)
{
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

static inline int main_check_sdict(void)
{
	struct sdict sdict;
	sdict_ctor(&sdict, &vdict_int_int, 2, 32);

	for (int k = 0; k < 10; k += 2)
	{
		int v = k * k;
		struct pair* pair = sdict_add_copy(&sdict, &k, &v);
		if (pair != NULL)
			printf("add(%i, %i) => {%i, %i}\n", k, v, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("add(%i, %i) => failure\n", k, v);
	}

	bool r = sdict_scope_begin(&sdict);
	printf("scope_begin() => %s\n", (r ? "success" : "failure"));

	r = sdict_scope_begin(&sdict);
	printf("scope_begin() => %s\n", (r ? "success" : "failure"));

	for (int k = 1; k < 10; k += 2)
	{
		int v = k * k;
		struct pair* pair = sdict_add_copy(&sdict, &k, &v);
		if (pair != NULL)
			printf("add(%i, %i) => {%i, %i}\n", k, v, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("add(%i, %i) => failure\n", k, v);
	}

	for (int k = -1; k < 16; ++k)
	{
		struct pair* pair = sdict_find(&sdict, &k);
		if (pair != NULL)
			printf("find(%i) => {%i, %i}\n", k, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("find(%i) => none\n", k);
	}

	for (int k = 4; k < 9; ++k)
	{
		bool r = sdict_remove(&sdict, &k);
		printf("remove(%i) => %s\n", k, (r ? "success" : "failure"));
	}

	for (int k = -1; k < 16; ++k)
	{
		struct pair* pair = sdict_find(&sdict, &k);
		if (pair != NULL)
			printf("find(%i) => {%i, %i}\n", k, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("find(%i) => none\n", k);
	}

	r = sdict_scope_end(&sdict);
	printf("scope_end() => %s\n", (r ? "success" : "failure"));

	r = sdict_scope_end(&sdict);
	printf("scope_end() => %s\n", (r ? "success" : "failure"));

	for (int k = -1; k < 16; ++k)
	{
		struct pair* pair = sdict_find(&sdict, &k);
		if (pair != NULL)
			printf("find(%i) => {%i, %i}\n", k, *PAIR_KEY(pair, int), *PAIR_VAL(pair, int));
		else
			printf("find(%i) => none\n", k);
	}

	sdict_dtor(&sdict);
	return 0;
}

void int_print(int const* i, FILE* stream) { fprintf(stream, "%i", *i); }
