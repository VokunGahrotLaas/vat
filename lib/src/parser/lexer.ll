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

	namespace vat::parser::lexer {

	vat::parser::yyParser::symbol_type make_NUMBER(const std::string &s, const vat::parser::yyParser::location_type& loc);

	} // namespace vat::parser::lexer
%}

id      [a-zA-Z_][a-zA-Z_0-9]*
inv_id  [0-9][a-zA-Z_][a-zA-Z_0-9]*
int     [0-9]{1,9}
inv_int [0-9]{10,}
blank   [ \t\r]

%%
%{
	vat::parser::location& loc = driver.location();
	loc.step();
%}

{blank}+   loc.step();
\n+        loc.lines(YYLeng()); loc.step();

"let"       return MAKE_TOKEN_NOARG(LET);
"fn"       return MAKE_TOKEN_NOARG(FN);
"if"       return MAKE_TOKEN_NOARG(IF);
"then"       return MAKE_TOKEN_NOARG(THEN);
"else"       return MAKE_TOKEN_NOARG(ELSE);
"true"       return MAKE_TOKEN_NOARG(TRUE);
"false"       return MAKE_TOKEN_NOARG(FALSE);

"="        return MAKE_TOKEN(ASSIGN, std::string{YYText()});
"-"        return MAKE_TOKEN(MINUS, std::string{YYText()});
"+"        return MAKE_TOKEN(PLUS, std::string{YYText()});
"*"        return MAKE_TOKEN(STAR, std::string{YYText()});
"/"        return MAKE_TOKEN(SLASH, std::string{YYText()});
"%"        return MAKE_TOKEN(MOD, std::string{YYText()});
"**"       return MAKE_TOKEN(POWER, std::string{YYText()});
";"        return MAKE_TOKEN(SEMICOLON, std::string{YYText()});
","        return MAKE_TOKEN(COMMA, std::string{YYText()});
"("        return MAKE_TOKEN(LPAREN, std::string{YYText()});
")"        return MAKE_TOKEN(RPAREN, std::string{YYText()});
"{"        return MAKE_TOKEN(LBRACE, std::string{YYText()});
"}"        return MAKE_TOKEN(RBRACE, std::string{YYText()});

{inv_id}   throw vat::parser::yyParser::syntax_error(loc, "invalid identifier: " + std::string{YYText()});
{id}       return MAKE_TOKEN(IDENTIFIER, std::string{YYText()});
{inv_int}  throw vat::parser::yyParser::syntax_error(loc, "invalid integer: " + std::string{YYText()});
{int}      return vat::parser::lexer::make_NUMBER(YYText(), loc);

<<EOF>>    return MAKE_TOKEN_NOARG(EOF);

.          throw vat::parser::yyParser::syntax_error(loc, "invalid character: " + std::string{YYText()});
%%

auto vat::parser::lexer::make_NUMBER(std::string const& s, yyParser::location_type const& loc) -> yyParser::symbol_type
{
	int old_errno = errno;
	errno = 0;
	long n = std::strtol(s.c_str(), NULL, 10);
	if (!(INT_MIN <= n && n <= INT_MAX && errno == 0))
		throw yyParser::syntax_error(loc, "integer is out of range: " + s);
	errno = old_errno;
	return MAKE_TOKEN(NUMBER, static_cast<int>(n));
}
