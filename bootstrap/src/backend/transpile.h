#pragma once

// bootstrap
#include "parser/ast.h"
#include "post/module.h"

bool transpile_c_prelude(FILE* stream, struct dict* modules);
bool transpile_c_exports(struct ast* ast, FILE* stream, struct dict* modules);
bool transpile_c_symbols(struct ast* ast, FILE* stream, struct dict* modules);
bool transpile_c_code(struct ast* ast, FILE* stream, struct dict* modules);
