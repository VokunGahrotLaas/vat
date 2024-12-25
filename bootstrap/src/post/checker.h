#pragma once

// bootstrap
#include "parser/ast.h"

struct checker
{
	bool error;
};

bool checker_ctor(struct checker* checker);
void checker_dtor(struct checker* checker);

bool checker_check(struct checker* checker, struct ast* ast);
