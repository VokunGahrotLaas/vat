#pragma once

#include <vat/ast/astdef.h>
#include <vat/ast/module.h>

typedef struct VatAst {
	VatAstNode* root;
} VatAst;

attr_nonnull(1)
VatAst* VatAst_parse(VatFile* in) {
	VatAst* ast = vat_xmalloc(sizeof(VatAst));
	ast->root = VatAst_parse_module(in);
	return ast;
}

attr_nonnull(1)
void VatAst_display(VatAst* ast, FILE* out) {
	fputs("{ Ast root=", out);
	if (ast->root != NULL)
		VatAstNode_display(ast->root, out);
	else
		fputs("NULL", out);
	fputs(" }\n", out);
}

void VatAst_delete(VatAst* self) {
	VatAstNode_delete(self->root);
	vat_free(self);
}
