#pragma once

#include "vatdef.h"
#include "file.h"
#include "vector.h"
#include "string.h"

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

bool vat_is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
bool vat_is_newline(char c) { return c == '\n'; }
bool vat_is_digit(char c) { return '0' <= c && c <= '9'; }
bool vat_is_alpha(char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
bool vat_is_alphanum(char c) { return vat_is_digit(c) || vat_is_alpha(c); }

attr_returns_nonnull attr_nonnull(1)
void* VatAst_parse_int(attr_unused VatFile* in) { return NULL; }
attr_returns_nonnull attr_nonnull(1)
void* VatAst_parse_str(attr_unused VatFile* in) { return NULL; }
attr_returns_nonnull attr_nonnull(1)
void* VatAst_parse_char(attr_unused VatFile* in) { return NULL; }

VatAstNode* VatAst_parse_single_comment(VatFile* in, char beg) {
	VatFile_skip_predicate(in, vat_is_whitespace);
	char* str = VatFile_gets_until_predicate(in, vat_is_newline);
	vat_strip_end(str, vat_is_whitespace);
	VatAstSingleComment* comment = VatAstSingleComment_new();
	comment->str = str;
	comment->beg = beg;
	return (VatAstNode*)comment;
}

attr_returns_nonnull attr_nonnull(1)
void* VatAst_parse_multi_comment(attr_unused VatFile* in) { return NULL; }

#define vat_push_ast_node(list, node) VatVector_push(VatAstNode*, list, vat_addr_temp(VatAstNode*, node))

attr_returns_nonnull attr_nonnull(1)
VatAstNode* VatAst_parse_module(attr_unused VatFile* in) {
	VatAstModule* module = VatAstModule_new();

	while (!VatFile_eof(in)) {
		char c = VatFile_get(in);
		if (c == EOF) break;
		if (vat_is_whitespace(c)) continue;
		if (vat_is_digit(c)) {
			VatFile_unget(in, c);
			vat_push_ast_node(module->nodes, VatAst_parse_int(in));
		} else if (vat_is_alpha(c)) {
			VatFile_unget(in, c);
			vat_push_ast_node(module->nodes, VatAst_parse_str(in));
		} else if (c == '/') {
			// if (VatFile_eof(file)) vat_error("unexpected eof");
			c = VatFile_get(in);
			if (c == '/')
				vat_push_ast_node(module->nodes, VatAst_parse_single_comment(in, c));
			else if (c == '*')
				vat_push_ast_node(module->nodes, VatAst_parse_multi_comment(in));
			else vat_error("unexpected character '%c'\n", c);
		} else if (c == '#') {
			vat_push_ast_node(module->nodes, VatAst_parse_single_comment(in, c));
		} else if (c == ';') {
		} else vat_error("unexpected character '%c'\n", c);
		// VatAstModule_display((VatAstNode*)module, stdout); puts("");
	}

	return (VatAstNode*)module;
}

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
