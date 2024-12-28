#include "post/binder.h"

// bootstrap
#include "post/visitor.h"

VPAIR(vpair_str_past, &vtype_cv, &vtype_past);
VDICT(vdict_str_past, struct cv, struct ast*, &vpair_str_past);

static bool binder_bind_ast(struct binder* binder, struct ast* ast);
static bool binder_prebind_ast(struct binder* binder, struct ast* ast);

bool binder_ctor(struct binder* binder)
{
	binder->error = false;
	return true;
}

void binder_dtor(UNUSED struct binder* binder) {}

bool binder_bind(struct binder* binder, struct ast* ast)
{
	sdict_ctor(&binder->vars, &vdict_str_past, 16, 16);
	struct ast* n = NULL;
	struct cv prelude[] = {
		cv_cstr("int"),
		cv_cstr("void"),
		cv_cstr("printf"),
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
	case AST_ERROR: FALLTHROUGH;
	case AST_NUMLIT: FALLTHROUGH;
	case AST_STRLIT: FALLTHROUGH;
	case AST_UNARY: FALLTHROUGH;
	case AST_CALL: FALLTHROUGH;
	case AST_SEXP: FALLTHROUGH;
	case AST_VARDEC:
	case AST_RET: return visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast);
	case AST_SEQ:
		sdict_scope_begin(&binder->vars);
		bool r = true;
		if (binder->first_seq)
		{
			visit_ast(binder, ast, (visit_ast_t*)&binder_prebind_ast);
			struct cv name = cv_cstr("main");
			struct pair const* pair = sdict_cfind(&binder->vars, &name);
			if (pair == NULL)
			{
				warnx("binder: missing main function");
				binder->error = true;
			}
			else if ((*PAIR_CVAL(pair, struct ast*))->ast_type != AST_FNDEC)
			{
				warnx("binder: symbol main is not a function");
				binder->error = true;
			}
			binder->first_seq = false;
		}
		r = visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast) && r;
		sdict_scope_end(&binder->vars);
		return r;
	case AST_FNDEC: {
		sdict_scope_begin(&binder->vars);
		bool r = visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast);
		sdict_scope_end(&binder->vars);
		return r;
	}
	case AST_WORD: {
		struct cv name = cv_str(&ast->value.word.str);
		struct pair const* pair = sdict_cfind(&binder->vars, &name);
		if (pair)
		{
			ast->value.word.dec = *PAIR_CVAL(pair, struct ast*);
			return true;
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
	case AST_SEXP: FALLTHROUGH;
	case AST_SEQ: FALLTHROUGH;
	case AST_WORD: FALLTHROUGH;
	case AST_RET: return true;
	case AST_VARDEC: {
		struct cv name = cv_str(&ast->value.vardec.name->value.word.str);
		return sdict_add_copy(&binder->vars, &name, &ast);
	}
	case AST_FNDEC: {
		struct cv name = cv_str(&ast->value.fndec.name->value.word.str);
		return sdict_add_copy(&binder->vars, &name, &ast);
	}
	};
	UNREACHABLE();
}
