#pragma once

#include "ast/astdef.h"
#include "ast/module.h"
#include "ast/single_comment.h"

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
