#pragma once

// bootstrap
#include "parser/ast.h"

struct binder
{
	bool error;
};

bool binder_ctor(struct binder* binder);
void binder_dtor(struct binder* binder);

bool binder_bind(struct binder* binder, struct ast* ast);
