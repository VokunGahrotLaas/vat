# VokunAavroTinvaak Syntax

## EBNF Grammar

```ebnf
(* tokens *)

EOF = ? end of file ?;

NEWLINE = ? new line ?;

NUMLIT = { _DIGIT };

STRLIT = "\"" { ? any ? } "\""

WORD = (_LETTER - _DIGIT) { _LETTER };

(* token rules *)

_DIGIT = ? /[0-9]/ ?;

_LETTER = ? /[a-zA-Z0-9_]/ ?;

_STRLIT_CHAR =
  ? any ? - "\"" - "\\"
| "\\" "\""
| "\\" "\\"
| "\\" "n"
| "\\" "t"
;

empty = ? empty ?;

(* input rules *)

parse_statement =
  statements NEWLINE
| statements EOF
;

parse_program = { statements NEWLINE } statements EOF;

(* statement rules *)

statements = { statement };

statement =
  sexp
| var_dec
| fun_dec
| ret
;

sexp = exp ";";

ret = "ret" exp ";";

var_dec = "let" lexp [ ":" texp ] "=" exp ";";

fun_dec = "fn" var "(" fun_args ")" [ "->" texp ] block;

fun_args = [ exp [ ":" texp ] { "," exp [ ":" texp ] } ];

block =
  statement
| "{" { statements NEWLINE } statements "}"
;

(* exp rules *)

exp =
  NUMLIT
| STRLIT
| lexp
| ops
| call_exp
;

call_exp = var "(" [ exp { "," exp } ] ")";

texp = var;

lexp = var;

var = word;

ops =
  "(" exp ")"
| "+" exp
| "-" exp
;
```

## Finished

* functions

## Current tasks

* arithmetic operators
* basic types

## TODO

* pointers
* structs
* tagged unions
* defer
* attributes (c\_name)
* external
* modules
* libc bindings/wrappers module ?
* standard lib module
  * str
  * list
  * dict
  * set
