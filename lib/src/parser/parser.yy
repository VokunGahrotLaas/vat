
%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.2"

%header
%locations

%define api.namespace {vat::parser}
%define api.parser.class {yyParser}

%define api.value.type variant
%define api.token.constructor
%define api.token.raw
%define api.token.prefix {TOK_}

%define parse.assert
%define parse.trace
%define parse.error detailed
%define parse.lac full

/* This block of code will go into the header file generated by bison */
%code requires{
  // STL
  #include <memory>

  // vat
	#include <vat/parser/driver.hh>
  #include <vat/ast/all.hh>
}

// The parsing context.
%param { vat::parser::Driver& driver }

%code {
  // vat
	#include <vat/parser/lexer.hh>
  #include <vat/ast/print_visitor.hh>

  using namespace vat::ast;
  using namespace vat;

	#undef yylex
	#define yylex driver.lexer().yylex
}

// keywords
%token
  LET "let"
  FN "fn"
  IF "if"
  THEN "then"
  ELSE "else"
  TRUE "true"
  FALSE "false"
;

// operators
%token
  <std::string> ASSIGN    "="
  <std::string> MINUS     "-"
  <std::string> PLUS      "+"
  <std::string> STAR      "*"
  <std::string> SLASH     "/"
  <std::string> MOD       "%"
  <std::string> POWER     "**"
  <std::string> SEMICOLON ";"
  <std::string> COMMA     ","
  <std::string> LPAREN    "("
  <std::string> RPAREN    ")"
  <std::string> LBRACE    "{"
  <std::string> RBRACE    "}"
;

// exps
%token
  <std::string> IDENTIFIER "identifier"
  <int> NUMBER "number"
;

%token EOF 0 "end of file";

%nterm <vat::ast::SharedSeqExp> exps_plural seq_exp fn_args fn_args.rec;
%nterm <vat::ast::SharedExp> exps exp rhs_exp block_exp;
%nterm <vat::ast::SharedName> name lhs_exp;
%nterm <vat::ast::SharedBool> bool_exp;
%nterm <vat::ast::SharedUnit> unit_exp;

%left ";"
%precedence "then"
%precedence "else"
%left "=";
%left "+" "-";
%left "*" "/" "%";
%precedence POS NEG;
%right "**";
%precedence "(";

%printer { driver.yyout() << $$; } <std::string> <int>;
%printer {
  vat::ast::PrintVisitor pv{driver.yyout(), true};
  pv(*$$);
} <vat::ast::SharedExp>;

%%
%start input;
input: exps { driver.set_result($1); };

exps:
  exps_plural { $$ = $1; }
| exp
;

exps_plural:
  exps_plural ";" exp { $$ = $1; $$->location(@$); $$->push_back($3); }
| exp ";" exp        { $$ = std::make_shared<SeqExp>(@$, std::vector<SharedExp>{$1, $3}); }
;

seq_exp:
  %empty { $$ = std::make_shared<SeqExp>(@$, std::vector<SharedExp>{}); }
| exp    { $$ = std::make_shared<SeqExp>(@$, std::vector<SharedExp>{$1}); }
| exps_plural
;

exp:
  rhs_exp
| lhs_exp { $$ = $1; }
;

rhs_exp:
  "if" exp "then" exp                 { $$ = std::make_shared<IfExp>(@$, $2, $4); }
| "if" exp "then" exp "else" exp      { $$ = std::make_shared<IfExp>(@$, $2, $4, $6); }
| "let" name "=" exp                  { $$ = std::make_shared<LetExp>(@$, $2, $4); }
| "fn" name "(" fn_args ")" block_exp { $$ = std::make_shared<LetExp>(@$, $2, std::make_shared<FnExp>(@$, $4, $6)); }
| "fn" "(" fn_args ")" block_exp      { $$ = std::make_shared<FnExp>(@$, $3, $5); }
| lhs_exp "=" exp                     { $$ = std::make_shared<AssignExp>(@$, $1, $3); }
| exp "+" exp                         { $$ = std::make_shared<BinaryOp>(@$, BinaryOp::Add, $1, $3); }
| exp "-" exp                         { $$ = std::make_shared<BinaryOp>(@$, BinaryOp::Sub, $1, $3); }
| exp "*" exp                         { $$ = std::make_shared<BinaryOp>(@$, BinaryOp::Mul, $1, $3); }
| exp "/" exp                         { $$ = std::make_shared<BinaryOp>(@$, BinaryOp::Div, $1, $3); }
| exp "%" exp                         { $$ = std::make_shared<BinaryOp>(@$, BinaryOp::Mod, $1, $3); }
| "-" exp %prec NEG                   { $$ = std::make_shared<UnaryOp>(@$, UnaryOp::Neg, $2); }
| "+" exp %prec POS                   { $$ = std::make_shared<UnaryOp>(@$, UnaryOp::Pos, $2); }
| exp "**" exp                        { $$ = std::make_shared<BinaryOp>(@$, BinaryOp::Pow, $1, $3); }
| exp "(" seq_exp ")"                 { $$ = std::make_shared<CallExp>(@$, $1, $3); }
| "(" rhs_exp ")"                     { $$ = $2; }
| NUMBER                              { $$ = std::make_shared<Number>(@$, $1); }
| bool_exp                            { $$ = $1; }
| unit_exp                            { $$ = $1; }
| block_exp
;

lhs_exp:
  name            { $$ = $1; }
| "(" lhs_exp ")" { $$ = $2; }
;

bool_exp:
  TRUE { $$ = std::make_shared<Bool>(@$, true); }
| FALSE { $$ = std::make_shared<Bool>(@$, false); }
;

unit_exp:
  "(" ")" { $$ = std::make_shared<Unit>(@$); }
;

block_exp: "{" exps "}" { $$ = $2; };

name:
  IDENTIFIER      { $$ = std::make_shared<Name>(@$, $1); }
;

fn_args:
  %empty      { $$ = std::make_shared<SeqExp>(@$, std::vector<SharedExp>{}); }
| fn_args.rec
;

fn_args.rec:
  name             { $$ = std::make_shared<SeqExp>(@$, std::vector<SharedExp>{$1}); }
| fn_args "," name { $$ = $1; $$->location(@$); $$->push_back($3); }
;
%%

void vat::parser::yyParser::error(const location_type& l, const std::string& m)
{
	driver.yyout() << l << ": " << m << std::endl;
}
