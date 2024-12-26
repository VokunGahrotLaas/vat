#include "post/visitor.h"

bool visit_ast(void* visitor, struct ast* ast, visit_ast_t* func)
{
	switch (ast->ast_type)
	{
	case AST_ERROR: FALLTHROUGH;
	case AST_NUMLIT: FALLTHROUGH;
	case AST_STRLIT: FALLTHROUGH;
	case AST_WORD: return true;
	case AST_UNARY: return (*func)(visitor, ast->value.unary.rhs);
	case AST_CALL: {
		bool r = true;
		r = (*func)(visitor, ast->value.call.fun) && r;
		struct list* args = &ast->value.call.args;
		for (size_t i = 0; i < args->size; ++i)
			r = (*func)(visitor, *LIST_GET(args, struct ast*, i)) && r;
		return r;
	}
	case AST_SEQ: {
		bool r = true;
		struct list* list = &ast->value.seq.list;
		for (size_t i = 0; i < list->size; ++i)
			r = (*func)(visitor, *LIST_GET(list, struct ast*, i)) && r;
		return r;
	}
	case AST_SEXP: return (*func)(visitor, ast->value.sexp.exp);
	case AST_RET: return (*func)(visitor, ast->value.ret.exp);
	case AST_VARDEC: {
		bool r = true;
		r = (*func)(visitor, ast->value.vardec.texp) && r;
		r = (*func)(visitor, ast->value.vardec.name) && r;
		r = (*func)(visitor, ast->value.vardec.exp) && r;
		return r;
	}
	case AST_FNDEC: {
		bool r = true;
		r = (*func)(visitor, ast->value.fndec.name) && r;
		struct list* targs = &ast->value.fndec.targs;
		for (size_t i = 0; i < targs->size; ++i)
			r = (*func)(visitor, *LIST_GET(targs, struct ast*, i)) && r;
		struct list* args = &ast->value.fndec.args;
		for (size_t i = 0; i < args->size; ++i)
			r = (*func)(visitor, *LIST_GET(args, struct ast*, i)) && r;
		r = (*func)(visitor, ast->value.fndec.texp) && r;
		r = (*func)(visitor, ast->value.fndec.exp) && r;
		return r;
	}
	};
	UNREACHABLE();
}
