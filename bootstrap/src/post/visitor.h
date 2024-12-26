#pragma once

// bootstrap
#include "parser/ast.h"

typedef bool visit_ast_t(void* visitor, struct ast* ast);

bool visit_ast(void* visitor, struct ast* ast, visit_ast_t* func);
