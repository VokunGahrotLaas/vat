#pragma once

// bootstrap
#include "parser/ast.h"
#include "utils/dict.h"

struct module
{
	// dict<cv, past>
	struct dict exports;
	struct dict privates;
	struct dict imports;
	// dict<cv, upmodule>
	struct dict children;
	// dict<cv, pmodule>
	struct dict depends;
	// other
	struct str name;
	struct str source_file;
	struct str compiled_file;
	struct ast* ast;
	struct module* parent;
};

struct module* module_init(struct cv name, struct ast* ast);
void module_free(struct module* module);
void module_print(struct module* module, FILE* stream);

void module_pdtor(struct module** module);
void module_pprint(struct module** module, FILE* stream);

struct ast* module_get_exported_dec(struct module* module, struct cv name);
struct ast* module_get_private_dec(struct module* module, struct cv name);
struct ast* module_get_depended_dec(struct module* module, struct cv module_name, struct cv name);

// impl

VTYPE(vtype_pmodule, struct module*, NULL, NULL, NULL, NULL, NULL, &module_pprint);
VTYPE(vtype_upmodule, struct module*, &module_pdtor, &copy_fail, NULL, NULL, NULL, &module_pprint);

VPAIR(vpair_cv_past, &vtype_cv, &vtype_past);
VDICT(vdict_cv_past, struct cv, struct ast*, &vpair_cv_past);

VPAIR(vpair_cv_pmodule, &vtype_cv, &vtype_pmodule);
VDICT(vdict_cv_pmodule, struct cv, struct module*, &vpair_cv_pmodule);

VPAIR(vpair_cv_upmodule, &vtype_cv, &vtype_upmodule);
VDICT(vdict_cv_upmodule, struct cv, struct module*, &vpair_cv_upmodule);
