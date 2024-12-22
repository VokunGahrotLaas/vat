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
	return 0;
}
