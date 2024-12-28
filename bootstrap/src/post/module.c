#include "post/module.h"

struct module* module_init(struct cv name)
{
	struct module* module = calloc(1, sizeof(*module));
	if (module == NULL) return module;
	str_of_cv(&module->name, name);
	str_ctor(&module->source_file, 16);
	str_ctor(&module->compiled_file, 16);
	dict_ctor(&module->exports_names, &vdict_cv_str, 16);
	dict_ctor(&module->exports, &vdict_cv_uptype, 16);
	dict_ctor(&module->children, &vdict_cv_upmodule, 16);
	dict_ctor(&module->depends, &vdict_cv_pmodule, 16);
	return module;
}

void module_free(struct module* module)
{
	if (module == NULL) return;
	str_dtor(&module->name);
	str_dtor(&module->source_file);
	str_dtor(&module->compiled_file);
	dict_dtor(&module->exports_names);
	dict_dtor(&module->exports);
	dict_dtor(&module->children);
	dict_dtor(&module->depends);
	free(module);
}

void module_print(struct module* module, FILE* stream)
{
	if (module->parent != NULL)
	{
		module_print(module->parent, stream);
		fputc('.', stream);
	}
	str_print(&module->name, stream);
}

void module_pdtor(struct module** module)
{
	if (!module) return;
	module_free(*module);
	*module = NULL;
}

void module_pprint(struct module** module, FILE* stream) { module_print(*module, stream); }

bool module_get_export(struct module* module, struct cv name) { return dict_find(&module->exports, &name) != NULL; }

struct module* module_get_child(struct module* module, struct cv name)
{
	struct pair* pair = dict_find(&module->children, &name);
	if (pair == NULL) return NULL;
	return *PAIR_VAL(pair, struct module*);
}
