#include "post/binder.h"

// bootstrap
#include "post/visitor.h"

static inline bool binder_bind_ast(struct binder* binder, struct ast* ast);

bool binder_ctor(struct binder* binder)
{
	binder->error = false;
	return true;
}

void binder_dtor(UNUSED struct binder* binder) {}

bool binder_bind(struct binder* binder, struct ast* ast) { return binder_bind_ast(binder, ast); }

static inline bool binder_bind_ast(struct binder* binder, struct ast* ast)
{
	switch (ast->ast_type)
	{
	case AST_ERROR: FALLTHROUGH;
	case AST_NUMLIT: FALLTHROUGH;
	case AST_STRLIT: FALLTHROUGH;
	case AST_UNARY: FALLTHROUGH;
	case AST_CALL: FALLTHROUGH;
	case AST_SEXP: FALLTHROUGH;
	case AST_RET: return visit_ast(binder, ast, (visit_ast_t*)&binder_bind_ast);
	case AST_SEQ: return true;
	case AST_VARDEC: return true;
	case AST_FNDEC: return true;
	case AST_WORD: return true;
	};
	UNREACHABLE();
}
