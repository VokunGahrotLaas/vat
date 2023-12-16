%option noyywrap nounput noinput batch debug
%option c++
%option yyclass="vat::parser::Lexer"
%option prefix="vat_parser_yy"

%{ /* -*- C++ -*- */
	#include <cerrno>
	#include <climits>
	#include <cstdlib>
	#include <string>
	#include <vat/parser/parser.yy.hh>
	#include <vat/parser/lexer.hh>
	#include <vat/parser/driver.hh>

	#undef YY_DECL
	#define YY_DECL vat::parser::yyParser::symbol_type vat::parser::Lexer::yylex(vat::parser::Driver& driver)

	// Code run each time a pattern is matched.
	#define YY_USER_ACTION  loc.columns(YYLeng());

	#define MAKE_TOKEN(Name, Arg) vat::parser::yyParser::make_##Name((Arg), loc)
	#define MAKE_TOKEN_NOARG(Name) vat::parser::yyParser::make_##Name(loc)

	vat::parser::yyParser::symbol_type make_NUMBER(const std::string &s, const vat::parser::yyParser::location_type& loc);
%}

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
blank [ \t\r]

%%
%{
	// A handy shortcut to the location held by the driver.
	vat::parser::location& loc = driver.location();
	// Code run each time yylex is called.
	loc.step();
%}

{blank}+   loc.step();
\n+        loc.lines(yyleng); loc.step();

"-"        return MAKE_TOKEN(MINUS, YYText());
"+"        return MAKE_TOKEN(PLUS, YYText());
"*"        return MAKE_TOKEN(STAR, YYText());
"/"        return MAKE_TOKEN(SLASH, YYText());
"("        return MAKE_TOKEN(LPAREN, YYText());
")"        return MAKE_TOKEN(RPAREN, YYText());
":="       return MAKE_TOKEN(ASSIGN, YYText());
"**"       return MAKE_TOKEN(POWER, YYText());

{int}      return make_NUMBER(YYText(), loc);
{id}       return MAKE_TOKEN(IDENTIFIER, YYText());

.          throw vat::parser::yyParser::syntax_error(loc, "invalid character: " + std::string(YYText()));

<<EOF>>    return MAKE_TOKEN_NOARG(YYEOF);
%%

vat::parser::yyParser::symbol_type make_NUMBER(const std::string &s, const vat::parser::yyParser::location_type& loc)
{
	int old_errno = errno;
	errno = 0;
	long n = std::strtol(s.c_str(), NULL, 10);
	if (!(INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
		throw vat::parser::yyParser::syntax_error(loc, "integer is out of range: " + s);
	errno = old_errno;
	return MAKE_TOKEN(NUMBER, static_cast<int>(n));
}
