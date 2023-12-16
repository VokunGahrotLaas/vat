#pragma once

#if !defined(yyFlexLexer) || yyFlexLexer != vat_parser_yyFlexLexer
#	undef yyFlexLexer
#	define yyFlexLexer vat_parser_yyFlexLexer
#	include <FlexLexer.h>
#endif

#include <vat/parser/parser.yy.hh>

namespace vat::parser
{

class Driver;

class Lexer : public yyFlexLexer
{
public:
	using yyFlexLexer::yyFlexLexer;
	using yyFlexLexer::yylex;

	vat::parser::yyParser::symbol_type yylex(Driver& driver);
};

} // namespace vat::parser
