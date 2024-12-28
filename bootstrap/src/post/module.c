#include "post/module.h"

bool module_ctor(struct module* module, struct cv name, struct ast* ast)
{
	module->ast = ast;
	str_of_cv(&module->name, name);
	str_ctor(&module->compiled_file, 16);
	dict_ctor(&module->depends, &vdict_cv_pmodule, 16);
	dict_ctor(&module->exports, &vdict_cv_past, 16);
	dict_ctor(&module->imports, &vdict_cv_past, 16);
	dict_ctor(&module->privates, &vdict_cv_past, 16);
	return true;
}

void module_dtor(struct module* module)
{
	ast_free(module->ast);
	str_dtor(&module->name);
	str_dtor(&module->compiled_file);
	dict_dtor(&module->depends);
	dict_dtor(&module->exports);
	dict_dtor(&module->imports);
	dict_dtor(&module->privates);
}

void module_print(struct module* module, FILE* stream) { str_print(&module->name, stream); }

struct ast* module_get_exported_dec(struct module* module, struct cv name)
{
	struct pair* pair = dict_find(&module->exports, &name);
	if (!pair) return NULL;
	return *PAIR_VAL(pair, struct ast*);
}

struct ast* module_get_private_dec(struct module* module, struct cv name)
{
	struct pair* pair = dict_find(&module->privates, &name);
	if (!pair) return NULL;
	return *PAIR_VAL(pair, struct ast*);
}

struct ast* module_get_depended_dec(struct module* module, struct cv module_name, struct cv name)
{
	struct pair* pair = dict_find(&module->depends, &module_name);
	if (!pair) return NULL;
	struct module* m = *PAIR_VAL(pair, struct module*);
	return module_get_exported_dec(m, name);
}
