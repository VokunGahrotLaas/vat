#include <vat/vatdef.h>
#include <vat/ast.h>

int main(attr_unused int argc, attr_unused char** argv) {
	VatFile_safe input = VatFile_open("main.vat", "r");
	attr_unused vat_safe_file output = stdout;

	/*
	if (argc <= 1) vat_usage(argc >= 1 ? *argv : "vat", true);
	if (argc > 1) {
		if (vat_eq(argv[1], "-h") || vat_eq(argv[1], "--help")) vat_usage(*argv, false);
		else if (vat_eq(argv[1], "-v") || vat_eq(argv[1], "--version")) vat_version();
		else if (vat_eq(argv[1], "-")) input = stdin;
		else input = vat_xfopen(argv[1], "r");
	}
	*/

	VatAst* ast = VatAst_parse(input);

	vat_push_ast_node(((VatAstModule*)ast->root)->nodes, (VatAstNode*)VatAstSingleComment_new());

	VatAst_display(ast, output);

	VatAst_delete(ast);

	return EXIT_SUCCESS;
}
