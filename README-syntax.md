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

attrs = { attr };

attr = "@" mvar [ "(" [ exp, [ "," exp ] ] ")" ];

statement = attrs statement.1;

statement.1 =
  sexp
| var_dec
| fun_dec
| ret
;

sexp = exp ";";

ret = "ret" exp ";";

mod_dec = "module" mvar ";";

imp_dec = "import" mvar [ "as" var ] ";";

var_dec = "let" var [ ":" texp ] "=" exp ";";

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

call_exp = mvar "(" [ exp { "," exp } ] ")";

texp = mvar;

lexp = mvar;

mvar = var { "." var };

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
* modules
* attributes (c\_name)

## TODO

* pointers
* structs
* tagged unions
* defer
* libc bindings/wrappers module ?
* standard lib module
  * str
  * list
  * dict
  * set
