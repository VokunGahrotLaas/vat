#pragma once

// bootstrap
#include "parser/ast.h"
#include "utils/sdict.h"

struct binder
{
	struct sdict vars;
	bool error;
	bool first_seq;
};

bool binder_ctor(struct binder* binder);
void binder_dtor(struct binder* binder);

bool binder_bind(struct binder* binder, struct ast* ast);
