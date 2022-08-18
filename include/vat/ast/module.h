#pragma once

#include <vat/ast/astdef.h>

typedef struct VatAstModule {
	VatAstVtbl const* vtbl;
	VatVector* nodes;
} VatAstModule;

attr_returns_nonnull attr_dealloc(VatAstNode_delete, 1) attr_wur
VatAstModule* VatAstModule_new(void);
void VatAstModule_ctor(VatAstModule* self) attr_nonnull(1);
void VatAstModule_dtor(VatAstNode* vself) attr_nonnull(1);
void VatAstModule_display(VatAstNode* vself, FILE* out) attr_nonnull(1, 2);

VatAstVtbl const* const VatAstModuleVtbl = &(VatAstVtbl const){
	.name = "Module",
	.size = sizeof(VatAstModule),
	.delete = VatAstNode_delete,
	.dtor = VatAstModule_dtor,
	.display = VatAstModule_display,
};

VatAstModule* VatAstModule_new(void) {
	VatAstModule* obj = vat_xmalloc(sizeof(VatAstModule));
	obj->vtbl = VatAstModuleVtbl;
	VatAstModule_ctor(obj);
	return obj;
}

void VatAstModule_ctor(VatAstModule* self) {
	self->nodes = VatVector_new();
}

void VatAstModule_dtor(VatAstNode* vself) {
	VatAstModule* self = (VatAstModule*)vself;
	for (size_t i = 0; i < VatVector_size(VatAstNode*, self->nodes); i++) {
		VatAstNode* node = *VatVector_get(VatAstNode*, self->nodes, i);
		VatAstNode_delete(node);
	}
	VatVector_delete(self->nodes);
}

void VatAstModule_display(VatAstNode* vself, FILE* out) {
	VatAstModule* self = (VatAstModule*)vself;
	printf("{ %s nodes=[", self->vtbl->name);
	for (size_t i = 0; i < VatVector_size(VatAstNode*, self->nodes); i++) {
		VatAstNode* node = *VatVector_get(VatAstNode*, self->nodes, i);
		fputc(' ', out);
		if (node != NULL) VatAstNode_display(node, out);
		else fputs("NULL", out);
		if (i < VatVector_size(VatAstNode*, self->nodes) - 1) fputc(',', out);
	}
	if (VatVector_size(VatAstNode*, self->nodes) > 0) fputc(' ', out);
	fputs("] }", out);
}
