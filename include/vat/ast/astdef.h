#pragma once

#include <vat/vatdef.h>
#include <vat/file.h>
#include <vat/vector.h>
#include <vat/string.h>

typedef struct VatAstNode VatAstNode;

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

#define vat_push_ast_node(list, node) VatVector_push(VatAstNode*, list, vat_addr_temp(VatAstNode*, node))

bool vat_is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
bool vat_is_newline(char c) { return c == '\n'; }
bool vat_is_digit(char c) { return '0' <= c && c <= '9'; }
bool vat_is_alpha(char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
bool vat_is_alphanum(char c) { return vat_is_digit(c) || vat_is_alpha(c); }
