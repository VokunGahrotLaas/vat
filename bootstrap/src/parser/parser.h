#pragma once

// bootstrap
#include "lexer/lexer.h"

struct parser
{
	struct lexer lexer;
};

bool parser_of_file(struct parser* parser, char const* filename);
void parser_dtor(struct parser* parser);
