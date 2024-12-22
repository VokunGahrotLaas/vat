// libc
#include <err.h>
#include <stdio.h>
// bootstrap
#include "parser/parser.h"

int main_lexer(char const* filename);
int main_parser(char const* filename);

int main(int argc, char** argv)
{
	if (argc != 2) errx(1, "invalid number of arguments");
	return main_parser(argv[1]);
}

int main_lexer(char const* filename)
{
	struct lexer lexer;
	lexer_of_file(&lexer, filename);
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

int main_parser(char const* filename)
{
	struct parser parser;
	parser_of_file(&parser, filename);
	struct ast* ast = parser_parse(&parser);
	ast_print(ast, stdout);
	ast_free(ast);
	int r = parser.lexer.error ? 1 : parser.error ? 2 : 0;
	parser_dtor(&parser);
	return r;
}
