#include "post/module_binder.h"

static inline bool module_binder_bind_moddec(struct module_binder* binder, struct ast* ast);
static inline bool module_binder_bind_exports(struct module_binder* binder, struct ast* ast);

bool module_binder_ctor(struct module_binder* binder, struct dict* modules, struct cv source_file)
{
	binder->error = false;
	binder->modules = modules;
	binder->source_file = source_file;
	return true;
}

void module_binder_dtor(UNUSED struct module_binder* binder)
{
	//
}

bool module_binder_bind(struct module_binder* binder, struct ast* ast)
{
	DBG_ASSERT(ast && ast->ast_type == AST_SEQ);
	struct list* seq = &ast->value.seq.list;
	for (size_t i = 0; i < seq->size; ++i)
		module_binder_bind_moddec(binder, *LIST_GET(seq, struct ast*, i));
	if (!binder->module)
	{
		warnx("module binder: missing module declaration");
		binder->error = true;
		return false;
	}
	str_pushcv(&binder->module->source_file, binder->source_file);
	for (size_t i = 0; i < seq->size; ++i)
		module_binder_bind_exports(binder, *LIST_GET(seq, struct ast*, i));
	return !binder->error;
}

static inline bool module_binder_bind_moddec(struct module_binder* binder, struct ast* ast)
{
	switch (ast->ast_type)
	{
	case AST_MODDEC:
		if (binder->module != NULL)
		{
			warnx("module binder: should only have one module declaration");
			binder->error = true;
			return false;
		}
		struct ast* var = ast->value.moddec.name;
		struct module* mod = NULL;
		struct dict* mods = binder->modules;
		while (var != NULL)
		{
			struct cv name = cv_str(&var->value.var.name);
			struct pair* pair = dict_find(mods, &name);
			if (!pair)
			{
				struct module* new_mod = module_init(name);
				struct cv new_name = cv_str(&new_mod->name);
				new_mod->parent = mod;
				mod = new_mod;
				dict_add_move(mods, &new_name, &new_mod);
			}
			else
				mod = *PAIR_VAL(pair, struct module*);
			mods = &mod->children;
			var = var->value.var.next;
		}
		if (mod->source_file.size > 0)
		{
			fprintf(stderr, "module binder: module \"");
			module_print(mod, stderr);
			fprintf(stderr, "\" was already defined in \"");
			str_print(&mod->compiled_file, stderr);
			fprintf(stderr, "\"\n");
			return false;
		}
		binder->module = mod;
		break;
	default: break;
	};
	return true;
}

static inline bool module_binder_bind_exports(struct module_binder* binder, struct ast* ast)
{
	struct cv const exp = cv_cstr("export");
	struct str* str_name = NULL;
	switch (ast->ast_type)
	{
	case AST_FNDEC:
		if (dict_find(&ast->attrs, &exp) == NULL) return true;
		str_name = &ast->value.fndec.name->value.var.name;
		break;
	case AST_VARDEC:
		if (dict_find(&ast->attrs, &exp) == NULL) return true;
		str_name = &ast->value.vardec.name->value.var.name;
		break;
	default: return true;
	};
	struct cv name = cv_str(str_name);
	struct type* type = NULL;
	dict_add_copy(&binder->module->exports_names, &name, str_name);
	dict_add_move(&binder->module->exports, &name, &type);
	return true;
}
