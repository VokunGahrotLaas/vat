#pragma once

// bootstrap
#include "parser/ast.h"

struct type_checker
{
	bool error;
};

bool type_checker_ctor(struct type_checker* checker);
void type_checker_dtor(struct type_checker* checker);

bool type_checker_check(struct type_checker* checker, struct ast* ast);
