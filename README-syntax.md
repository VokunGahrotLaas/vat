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
  exp ";"
| assign ";"
;

assign = "let" lexp opt_type "=" exp;

opt_type =
  empty
| ":" texp
;

(* exp rules *)

exp =
  NUMLIT
| STRLIT
| lexp
| ops
| lexp "(" call_args ")"
;

texp = var;

lexp = var;

var = word;

ops =
  "(" exp ")"
| "+" exp
| "-" exp
;

call_args =
  empty
| exp { "," exp }
;
```

## Finished

## Current tasks

* arithmetic operators
* basic types

## TODO

* pointers
* functions
* structs
* tagged unions
* attributes (c\_name)
* external
* modules
* libc bindings/wrappers module ?
* standard lib module
  * str
  * list
  * dict
  * set
