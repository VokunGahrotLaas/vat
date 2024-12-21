# VokunAavroTinvaak Syntax

## EBNF Grammar

```ebnf
(* tokens *)

EOF = ? end of file ?;

NEWLINE = ? new line ?;

NUMBER = { _DIGIT };

WORD = (_LETTER - _DIGIT) { _LETTER };

(* token rules *)

_DIGIT = ? /[0-9]/ ?;

_LETTER = ? /[a-zA-Z0-9_]/ ?;

(* input rules *)

parse_statement =
  statements NEWLINE
| statements EOF;

parse_program = ( statements [ NEWLINE ] ) + EOF;

(* other rules *)

statements = ( statement ) +;

statement = exp ";";

exp =
  number
| ops
| lexp
;

lexp = word;

ops =
  "(" exp ")"
| "+" exp
| "-" exp
;
```

## Finished

## Current tasks

* arithmetic operators

## TODO

* basic types
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
