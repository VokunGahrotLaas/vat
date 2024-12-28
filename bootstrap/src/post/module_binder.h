#pragma once

// bootstrap
#include "post/module.h"

struct module_binder
{
	struct dict* modules;
	struct module* module;
	struct cv source_file;
	bool error;
};

bool module_binder_ctor(struct module_binder* binder, struct dict* modules, struct cv source_file);
void module_binder_dtor(struct module_binder* binder);

bool module_binder_bind(struct module_binder* binder, struct ast* ast);
