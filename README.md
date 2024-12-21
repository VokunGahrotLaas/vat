# VokunAavroTinvaak

A compiler for a custom programming language called vat.

## Features

TBD

## Finished

* bootstrap compiler:
  * lexer

## TODO

* write a basic syntax
  * basic types
  * functions
  * structs
  * tagged unions
  * attributes (c\_name)
  * external
  * arithmetic operators
  * modules
  * libc bindings/wrappers module ?
  * standard lib module
    * str
    * list
    * dict
    * set
* write a bootstrap vatc in C
  * parser
  * type checker ?
  * transpile to c
  * parse args
  * tests
* write vatc in vat
  * lexer
  * parser
  * type checker
  * transpile to c
  * transpile to qbe
  * transpile to llvm
  * transpile to bytecode + vm
  * parse args
  * tests
* write a more interesting syntax
  * compile time functions
  * overloading
  * classes
  * interfaces (haskell's typeclass not java's interfaces)
  * first class functions
  * lambdas
  * first class types ?
  * etc...
