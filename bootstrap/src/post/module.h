#pragma once

// bootstrap
#include "post/type.h"
#include "utils/dict.h"
#include "utils/str.h"

struct module
{
	// dict<cv, str>
	struct dict exports_names;
	// dict<cv, uptype>
	struct dict exports;
	// dict<cv, upmodule>
	struct dict children;
	// dict<cv, pmodule>
	struct dict depends;
	// other
	struct str name;
	struct str source_file;
	struct str compiled_file;
	struct module* parent;
};

struct module* module_init(struct cv name);
void module_free(struct module* module);
void module_print(struct module* module, FILE* stream);

void module_pdtor(struct module** module);
void module_pprint(struct module** module, FILE* stream);

bool module_get_export(struct module* module, struct cv name);
struct module* module_get_child(struct module* module, struct cv name);

// impl

VTYPE(vtype_pmodule, struct module*, NULL, NULL, NULL, NULL, NULL, &module_pprint);
VTYPE(vtype_upmodule, struct module*, &module_pdtor, &copy_fail, NULL, NULL, NULL, &module_pprint);

VPAIR(vpair_cv_pmodule, &vtype_cv, &vtype_pmodule);
VDICT(vdict_cv_pmodule, struct cv, struct module*, &vpair_cv_pmodule);

VPAIR(vpair_cv_upmodule, &vtype_cv, &vtype_upmodule);
VDICT(vdict_cv_upmodule, struct cv, struct module*, &vpair_cv_upmodule);

VPAIR(vpair_cv_uptype, &vtype_cv, &vtype_uptype);
VDICT(vdict_cv_uptype, struct cv, struct type*, &vpair_cv_uptype);

VPAIR(vpair_cv_str, &vtype_cv, &vtype_str);
VDICT(vdict_cv_str, struct cv, struct str, &vpair_cv_str);
