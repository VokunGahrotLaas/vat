#include "vatdef.h"

typedef enum VatToken {
	IntLitteral,
	StrLitteral,
	CharLitteral,
	SingleComment,
	MultiComment,
} VatToken;

typedef struct VatAST {
	void* root;
} VatAST;

attr_always_inline bool vat_is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
attr_always_inline bool vat_is_newline(char c) { return c == '\n'; }
attr_always_inline bool vat_is_digit(char c) { return '0' <= c && c <= '9'; }
attr_always_inline bool vat_is_alpha(char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
attr_always_inline bool vat_is_alphanum(char c) { return vat_is_digit(c) || vat_is_alpha(c); }

attr_returns_nonnull attr_nonnull(1)
void* VatAST_parse_int(attr_unused FILE* in) { return NULL; }
attr_returns_nonnull attr_nonnull(1)
void* VatAST_parse_str(attr_unused FILE* in) { return NULL; }
attr_returns_nonnull attr_nonnull(1)
void* VatAST_parse_char(attr_unused FILE* in) { return NULL; }
attr_returns_nonnull attr_nonnull(1)
void* VatAST_parse_single_comment(attr_unused FILE* in) { return NULL; }
attr_returns_nonnull attr_nonnull(1)
void* VatAST_parse_multi_comment(attr_unused FILE* in) { return NULL; }

attr_nonnull(1)
VatAST* VatAST_parse(FILE* in) {
	VatAST* ast = vat_xmalloc(sizeof(VatAST));
	ast->root = NULL;

	while (!feof(in)) {
		char c = fgetc(in);
		if (vat_is_whitespace(c)) continue;
		if (vat_is_digit(c)) {
			ungetc(c, in);
			ast->root = VatAST_parse_int(in);
		} else if (vat_is_alpha(c)) {
			ungetc(c, in);
			ast->root = VatAST_parse_str(in);
		} else if (c == '/') {
			if (feof(in)) vat_error("unexpected eof");
			c = fgetc(in);
			if (c == '/') ast->root = VatAST_parse_single_comment(in);
			else if (c == '*') ast->root = VatAST_parse_multi_comment(in);
			else vat_error("unexpected character '%c'\n", c);
		} else if (c == '#') {
			ast->root = VatAST_parse_single_comment(in);
		} else if (c == ';') {
		} else vat_error("unexpected character '%c'\n", c);
	}

	return ast;
}

attr_nonnull(1)
void VatAST_display(VatAST* ast) {
	printf("root: %p\n", ast->root);
}
