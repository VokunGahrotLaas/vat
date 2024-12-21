// libc
#include <err.h>
#include <stdio.h>
// bootstrap
#include "lexer/lexer.h"

int main(int argc, char** argv)
{
	if (argc != 2) errx(1, "invalid number of arguments");
	struct lexer lexer;
	lexer_from_file(&lexer, argv[1]);
	while (lexer_peek(&lexer).type != TOKEN_EOF)
	{
		struct token token = lexer_pop(&lexer);
		location_print(&token.loc, stdout);
		fputs(": ", stdout);
		token_print(&token, stdout);
		fputc('\n', stdout);
		token_dtor(&token);
	}
	lexer_dtor(&lexer);
	return 0;
}
