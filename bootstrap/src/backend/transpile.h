#pragma once

// bootstrap
#include "parser/ast.h"
#include "post/module.h"

bool transpile_c(struct ast* ast, char const* filename, struct dict* modules);
