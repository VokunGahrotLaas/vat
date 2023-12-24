#pragma once

#if !defined(yyFlexLexer) || yyFlexLexer != vat_parse_yyFlexLexer
#	undef yyFlexLexer
#	define yyFlexLexer vat_parse_yyFlexLexer
#	include <FlexLexer.h>
#endif

#include <vat/parse/parser.yy.hh>

namespace vat::parse
{

class Driver;

class Lexer : public yyFlexLexer
{
public:
	using yyFlexLexer::yyFlexLexer;
	using yyFlexLexer::yylex;

	vat::parse::yyParser::symbol_type yylex(Driver& driver);
};

} // namespace vat::parse
