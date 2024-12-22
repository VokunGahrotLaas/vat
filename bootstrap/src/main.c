// libc
#include <err.h>
#include <stdio.h>
// bootstrap
#include "parser/parser.h"

int main(int argc, char** argv)
{
	if (argc != 2) errx(1, "invalid number of arguments");

	puts("lexer:");
	struct lexer lexer;
	lexer_of_file(&lexer, argv[1]);
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
	lexer_dtor(&lexer);

	puts("parser:");
	struct parser parser;
	parser_of_file(&parser, argv[1]);
	struct ast* ast = parser_parse(&parser);
	ast_print(ast, stdout);
	ast_free(ast);
	parser_dtor(&parser);

	return 0;
}
