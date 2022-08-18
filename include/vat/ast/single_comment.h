#pragma once

#include <vat/ast/astdef.h>

typedef struct VatAstSingleComment {
	VatAstVtbl const* vtbl;
	char* str;
	char beg;
} VatAstSingleComment;

attr_returns_nonnull attr_dealloc(VatAstNode_delete, 1) attr_wur
VatAstSingleComment* VatAstSingleComment_new(void);
void VatAstSingleComment_ctor(VatAstSingleComment* self) attr_nonnull(1);
void VatAstSingleComment_dtor(VatAstNode* vself) attr_nonnull(1);
void VatAstSingleComment_display(VatAstNode* vself, FILE* out) attr_nonnull(1, 2);

VatAstVtbl const* const VatAstSingleCommentVtbl = &(VatAstVtbl const){
	.name = "Single Comment",
	.size = sizeof(VatAstSingleComment),
	.delete = VatAstNode_delete,
	.dtor = VatAstSingleComment_dtor,
	.display = VatAstSingleComment_display,
};

VatAstSingleComment* VatAstSingleComment_new(void) {
	VatAstSingleComment* obj = vat_xmalloc(sizeof(VatAstSingleComment));
	obj->vtbl = VatAstSingleCommentVtbl;
	VatAstSingleComment_ctor(obj);
	return obj;
}

void VatAstSingleComment_ctor(VatAstSingleComment* self) {
	self->str = NULL;
	self->beg = '\0';
}

void VatAstSingleComment_dtor(VatAstNode* vself) {
	VatAstSingleComment* self = (VatAstSingleComment*)vself;
	vat_free(self->str);
}

void VatAstSingleComment_display(VatAstNode* vself, FILE* out) {
	VatAstSingleComment* self = (VatAstSingleComment*)vself;
	fprintf(out, "{ %s str=", self->vtbl->name);
	if (self->str != NULL) {
		vat_safe_str esc = vat_escaped_str(self->str, vat_escaped_ascii);
		fprintf(out, "\"%s\"", esc);
	} else fputs("NULL", out);
	fprintf(out, " char='%s' }", vat_escaped_ascii(self->beg));
}

VatAstNode* VatAst_parse_single_comment(VatFile* in, char beg) {
	VatFile_skip_predicate(in, vat_is_whitespace);
	char* str = VatFile_gets_until_predicate(in, vat_is_newline);
	vat_strip_end(str, vat_is_whitespace);
	VatAstSingleComment* comment = VatAstSingleComment_new();
	comment->str = str;
	comment->beg = beg;
	return (VatAstNode*)comment;
}
