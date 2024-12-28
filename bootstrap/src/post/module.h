#pragma once

// bootstrap
#include "parser/ast.h"
#include "utils/dict.h"

struct module
{
	struct dict exports;
	struct dict privates;
	struct dict imports;
	struct dict depends;
	struct str name;
	struct str compiled_file;
	struct ast* ast;
};

bool module_ctor(struct module* module, struct cv name, struct ast* ast);
void module_dtor(struct module* module);
void module_print(struct module* module, FILE* stream);

struct ast* module_get_exported_dec(struct module* module, struct cv name);
struct ast* module_get_private_dec(struct module* module, struct cv name);
struct ast* module_get_depended_dec(struct module* module, struct cv module_name, struct cv name);

// impl

VTYPE(vtype_module, struct module, &module_dtor, &copy_fail, NULL, NULL, NULL, &module_print);
VTYPE(vtype_pmodule, struct module*, NULL, NULL, NULL, NULL, NULL, NULL);

VLIST(vlist_module, struct module, &vtype_module);

VPAIR(vpair_cv_past, &vtype_cv, &vtype_past);
VDICT(vdict_cv_past, struct cv, struct ast*, &vpair_cv_past);

VPAIR(vpair_cv_pmodule, &vtype_cv, &vtype_pmodule);
VDICT(vdict_cv_pmodule, struct cv, struct module*, &vpair_cv_pmodule);
