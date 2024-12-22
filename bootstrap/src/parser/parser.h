#pragma once

// bootstrap
#include "lexer/lexer.h"
#include "parser/ast.h"

struct parser
{
	struct lexer lexer;
	bool error;
};

bool parser_of_file(struct parser* parser, char const* filename);
void parser_dtor(struct parser* parser);

struct ast* parser_parse(struct parser* parser);
