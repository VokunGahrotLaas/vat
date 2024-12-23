#pragma once

// bootstrap
#include "parser/ast.h"

bool transpile_c(struct ast* ast, char const* filename);
