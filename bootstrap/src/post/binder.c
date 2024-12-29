#include "post/binder.h"

// bootstrap
#include "post/visitor.h"

VPAIR(vpair_str_past, &vtype_cv, &vtype_past);
VDICT(vdict_str_past, struct cv, struct ast*, &vpair_str_past);

static bool binder_bind_ast(struct binder* binder, struct ast* ast);
static bool binder_prebind_ast(struct binder* binder, struct ast* ast);
static inline struct module* binder_get_module(struct binder* binder, struct ast* var);
static bool binder_bind_dec_var(struct binder* binder, struct ast* dec, struct ast* var);
static bool binder_bind_mod_var(struct binder* binder, struct module* mod, struct ast* var);

bool binder_ctor(struct binder* binder, struct dict* modules)
{
	binder->error = false;
	binder->modules = modules;
	return true;
}

void binder_dtor(UNUSED struct binder* binder) {}

bool binder_bind(struct binder* binder, struct ast* ast)
{
	sdict_ctor(&binder->vars, &vdict_str_past, 16, 16);
	struct ast* n = NULL;
	struct cv prelude[] = {
		cv_cstr("int"), cv_cstr("str"), cv_cstr("void"), cv_cstr("size_t"), cv_cstr("ssize_t"),
	};
	for (size_t i = 0; i < ARR_SIZE(prelude); ++i)
		sdict_add_copy(&binder->vars, &prelude[i], &n);
	binder->first_seq = true;
	bool r = binder_bind_ast(binder, ast);
	sdict_dtor(&binder->vars);
	return r;
}

static bool binder_bind_ast(struct binder* binder, struct ast* ast)
{
	switch (ast->ast_type)
	{
	case AST_MODDEC: return true;
	case AST_ERROR: FALLTHROUGH;
	case AST_NUMLIT: FALLTHROUGH;
	case AST_STRLIT: FALLTHROUGH;
	case AST_UNARY: FALLTHROUGH;
	case AST_CALL: FALLTHROUGH;
	case AST_ATTR: FALLTHROUGH;
	case AST_SEXP: FALLTHROUGH;
	case AST_RET: return visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast);
	case AST_IMPDEC: {
		if (ast->value.impdec.as_name == NULL)
		{
			warnx("binder: \"as\" is needed for imports (for now)");
			binder->error = true;
			return false;
		}
		struct cv name = cv_str(&ast->value.impdec.as_name->value.var.name);
		bool r = sdict_add_copy(&binder->vars, &name, &ast);
		ast->value.impdec.module = binder_get_module(binder, ast->value.impdec.mod_name);
		return visit_ast(binder, ast->value.impdec.as_name, (visit_ast_t*)&binder_bind_ast) && r;
	}
	case AST_SEQ:
		sdict_scope_begin(&binder->vars);
		bool r = true;
		if (binder->first_seq)
		{
			r = visit_ast(binder, ast, (visit_ast_t*)&binder_prebind_ast) && r;
			binder->first_seq = false;
		}
		sdict_scope_begin(&binder->vars);
		r = visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast) && r;
		sdict_scope_end(&binder->vars);
		sdict_scope_end(&binder->vars);
		return r;
	case AST_VARDEC: {
		struct cv name = cv_str(&ast->value.vardec.name->value.var.name);
		bool r = sdict_add_copy(&binder->vars, &name, &ast);
		return visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast) && r;
	}
	case AST_FNDEC: {
		struct cv name = cv_str(&ast->value.fndec.name->value.var.name);
		bool r = sdict_add_copy(&binder->vars, &name, &ast);
		sdict_scope_begin(&binder->vars);
		r = visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast) && r;
		sdict_scope_end(&binder->vars);
		return r;
	}
	case AST_VAR: {
		struct cv name = cv_str(&ast->value.var.name);
		struct pair const* pair = sdict_cfind(&binder->vars, &name);
		if (pair)
		{
			ast->value.var.dec = *PAIR_CVAL(pair, struct ast*);
			return ast->value.var.next == NULL || binder_bind_dec_var(binder, ast->value.var.dec, ast->value.var.next);
		}
		warnx("binder: no such variable in scope \"%s\"", name.data);
		binder->error = true;
		return false;
	}
	};
	UNREACHABLE();
}

static bool binder_prebind_ast(struct binder* binder, struct ast* ast)
{
	switch (ast->ast_type)
	{
	case AST_ERROR: FALLTHROUGH;
	case AST_NUMLIT: FALLTHROUGH;
	case AST_STRLIT: FALLTHROUGH;
	case AST_UNARY: FALLTHROUGH;
	case AST_CALL: FALLTHROUGH;
	case AST_ATTR: FALLTHROUGH;
	case AST_SEXP: FALLTHROUGH;
	case AST_SEQ: FALLTHROUGH;
	case AST_VAR: FALLTHROUGH;
	case AST_MODDEC: FALLTHROUGH;
	case AST_IMPDEC: FALLTHROUGH;
	case AST_RET: return true;
	case AST_VARDEC: {
		struct cv name = cv_str(&ast->value.vardec.name->value.var.name);
		return sdict_add_copy(&binder->vars, &name, &ast);
	}
	case AST_FNDEC: {
		struct cv name = cv_str(&ast->value.fndec.name->value.var.name);
		return sdict_add_copy(&binder->vars, &name, &ast);
	}
	};
	UNREACHABLE();
}

static inline struct module* binder_get_module(struct binder* binder, struct ast* var)
{
	struct dict* mods = binder->modules;
	struct module* mod = NULL;
	for (; var; var = var->value.var.next)
	{
		struct cv name = cv_str(&var->value.var.name);
		struct pair* pair = dict_find(mods, &name);
		if (!pair)
		{
			fprintf(stderr, "binder: submodule %s does not exist", name.data);
			if (mod != NULL)
			{
				fputs(" in module ", stderr);
				module_print(mod, stderr);
			}
			fputc('\n', stderr);
			binder->error = true;
			return NULL;
		}
		mod = *PAIR_VAL(pair, struct module*);
		mods = &mod->children;
	}
	return mod;
}

static bool binder_bind_dec_var(struct binder* binder, struct ast* dec, struct ast* var)
{
	switch (dec->ast_type)
	{
	case AST_IMPDEC:
		DBG_ASSERT(dec->value.impdec.module != NULL);
		return binder_bind_mod_var(binder, dec->value.impdec.module, var);
	case AST_VARDEC:
		warnx("binder: cannot use the dot operator on variables");
		binder->error = true;
		return false;
	case AST_FNDEC:
		warnx("binder: cannot use the dot operator on functions");
		binder->error = true;
		return false;
	default: return false;
	}
}

static bool binder_bind_mod_var(struct binder* binder, struct module* mod, struct ast* var)
{
	for (; var; var = var->value.var.next)
	{
		struct cv name = cv_str(&var->value.var.name);
		struct pair* pair = dict_find(&mod->children, &name);
		if (pair != NULL)
		{
			if (var->value.var.next == NULL)
			{
				warnx("binder: cannot end dot sequence on a module");
				binder->error = true;
				return false;
			}
			mod = *PAIR_VAL(pair, struct module*);
			var->value.var.dec = NULL; // TODO
			continue;
		}
		pair = dict_find(&mod->exports, &name);
		if (pair != NULL)
		{
			if (var->value.var.next != NULL)
			{
				warnx("binder: cannot continue dot sequence on an object");
				binder->error = true;
				return false;
			}
			mod = *PAIR_VAL(pair, struct module*);
			var->value.var.dec = NULL; // TODO
			return true;
		}
		warnx("binder: %s not found in module", var->value.var.name.data);
		binder->error = true;
		return false;
	}
	UNREACHABLE();
}
