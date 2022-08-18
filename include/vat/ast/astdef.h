#pragma once

#include <vat/vatdef.h>
#include <vat/file.h>
#include <vat/vector.h>
#include <vat/string.h>

typedef struct VatAstNode VatAstNode;

typedef struct VatAst {
	VatAstNode* root;
} VatAst;

typedef void (*VatAstNode_delete_f)(VatAstNode* vself);
typedef void (*VatAstNode_dtor_f)(VatAstNode* vself) attr_nonnull(1);
typedef void (*VatAstNode_display_f)(VatAstNode* vself, FILE* out) attr_nonnull(1, 2);

typedef struct attr_packed VatAstVtbl {
	char const* name;
	size_t size;
	VatAstNode_delete_f delete;
	VatAstNode_dtor_f dtor;
	VatAstNode_display_f display;
} VatAstVtbl;

typedef struct VatAstNode {
	VatAstVtbl const* vtbl;
} VatAstNode;

void VatAstNode_delete(VatAstNode* vself) {
	if (vself == NULL) return;
	vself->vtbl->dtor(vself);
	vat_free(vself);
}

attr_nonnull(1, 2)
void VatAstNode_display(VatAstNode* vself, FILE* out) { vself->vtbl->display(vself, out); }
