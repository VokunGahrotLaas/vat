#pragma once

// bootstrap
#include "parser/ast.h"
#include "post/module.h"
#include "utils/sdict.h"

struct binder
{
	struct dict* modules;
	struct sdict vars;
	bool error;
	bool first_seq;
};

bool binder_ctor(struct binder* binder, struct dict* modules);
void binder_dtor(struct binder* binder);

bool binder_bind(struct binder* binder, struct ast* ast);
