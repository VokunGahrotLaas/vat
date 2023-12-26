%option noyywrap nounput noinput batch debug
%option c++
%option yyclass="vat::parse::Lexer"
%option prefix="vat_parse_yy"

%{ /* -*- C++ -*- */
	#include <cerrno>
	#include <climits>
	#include <cstdlib>
	#include <string>
	#include <vat/parse/parser.yy.hh>
	#include <vat/parse/lexer.hh>
	#include <vat/parse/driver.hh>

	#undef YY_DECL
	#define YY_DECL vat::parse::yyParser::symbol_type vat::parse::Lexer::yylex(vat::parse::Driver& driver)

	// Code run each time a pattern is matched.
	#define YY_USER_ACTION  driver.location().columns(YYLeng());

	#define MAKE_TOKEN(Name, Arg) vat::parse::yyParser::make_##Name((Arg), driver.location())
	#define MAKE_TOKEN_NOARG(Name) vat::parse::yyParser::make_##Name(driver.location())

	namespace vat::parse::lexer {

	yyParser::symbol_type make_NUMBER(Driver& driver, const std::string &s);
	yyParser::symbol_type make_ERROR(Driver& driver, std::string_view reason);

	} // namespace vat::parse::lexer
%}

id      [a-zA-Z_][a-zA-Z_0-9]*
inv_id  [0-9][a-zA-Z_][a-zA-Z_0-9]*
int     [0-9]{1,9}
inv_int [0-9]{10,}
blank   [ \t\r]

%%
%{
	driver.location().step();
%}

{blank}+   driver.location().step();
\n+        driver.location().lines(YYLeng()); driver.location().step();

"let"      return MAKE_TOKEN_NOARG(LET);
"rec"      return MAKE_TOKEN_NOARG(REC);
"fn"       return MAKE_TOKEN_NOARG(FN);
"if"       return MAKE_TOKEN_NOARG(IF);
"then"     return MAKE_TOKEN_NOARG(THEN);
"else"     return MAKE_TOKEN_NOARG(ELSE);
"true"     return MAKE_TOKEN_NOARG(TRUE);
"false"    return MAKE_TOKEN_NOARG(FALSE);

"="        return MAKE_TOKEN_NOARG(ASSIGN);
"=="       return MAKE_TOKEN_NOARG(EQ);
"!="       return MAKE_TOKEN_NOARG(NE);
"<"        return MAKE_TOKEN_NOARG(LT);
"<="       return MAKE_TOKEN_NOARG(LE);
">"        return MAKE_TOKEN_NOARG(GT);
">="       return MAKE_TOKEN_NOARG(GE);
"-"        return MAKE_TOKEN_NOARG(MINUS);
"+"        return MAKE_TOKEN_NOARG(PLUS);
"*"        return MAKE_TOKEN_NOARG(STAR);
"/"        return MAKE_TOKEN_NOARG(SLASH);
"%"        return MAKE_TOKEN_NOARG(MOD);
"**"       return MAKE_TOKEN_NOARG(POWER);
";"        return MAKE_TOKEN_NOARG(SEMICOLON);
":"        return MAKE_TOKEN_NOARG(COLON);
"->"       return MAKE_TOKEN_NOARG(ARROW);
","        return MAKE_TOKEN_NOARG(COMMA);
"("        return MAKE_TOKEN_NOARG(LPAREN);
")"        return MAKE_TOKEN_NOARG(RPAREN);
"{"        return MAKE_TOKEN_NOARG(LBRACE);
"}"        return MAKE_TOKEN_NOARG(RBRACE);
"!"        return MAKE_TOKEN_NOARG(EXCLAM);

{inv_id}   vat::parse::lexer::make_ERROR(driver, "invalid identifier: " + std::string{YYText()}); return MAKE_TOKEN(IDENTIFIER, std::string{YYText()});
{id}       return MAKE_TOKEN(IDENTIFIER, std::string{YYText()});
{inv_int}  vat::parse::lexer::make_ERROR(driver, "invalid integer: " + std::string{YYText()}); return MAKE_TOKEN(NUMBER, 0);
{int}      return vat::parse::lexer::make_NUMBER(driver, YYText());

<<EOF>>    return MAKE_TOKEN_NOARG(EOF);

.          return vat::parse::lexer::make_ERROR(driver, "invalid character: " + std::string{YYText()});
%%

auto vat::parse::lexer::make_NUMBER(Driver& driver, std::string const& s) -> yyParser::symbol_type
{
	int old_errno = errno;
	errno = 0;
	long n = std::strtol(s.c_str(), NULL, 10);
	if (!(INT_MIN <= n && n <= INT_MAX && errno == 0))
	{
		make_ERROR(driver, "integer is out of range: " + s);
		return MAKE_TOKEN(NUMBER, 0);
	}
	errno = old_errno;
	return MAKE_TOKEN(NUMBER, static_cast<int>(n));
}

auto vat::parse::lexer::make_ERROR(Driver& driver, std::string_view reason) -> yyParser::symbol_type
{
	driver.error(utils::ErrorType::Lexing) << reason;
	return MAKE_TOKEN_NOARG(ERROR);
}
