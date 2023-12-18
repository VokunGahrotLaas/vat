// STL
#include <iostream>

// vat
#include <variant>
#include <vat/ast/print_visitor.hh>
#include <vat/eval/ast.hh>
#include <vat/parser/parser.hh>

#include "vat/ast/fwd.hh"

int main(int argc, char** argv)
{
	int res = 0;
	vat::parser::Parser parser;
	vat::ast::PrintVisitor pv{ std::cout, true };
	vat::eval::AstEvaluator ae;
	for (int i = 1; i < argc; ++i)
		if (argv[i] == std::string("-p"))
			parser.set_trace_parsing(true);
		else if (argv[i] == std::string("-s"))
			parser.set_trace_scanning(true);
		else if (auto const ast = parser.parse(argv[i]))
		{
			pv(*ast);
			std::cout << std::endl;
			auto result = ae.eval(*ast);
			if (auto value = std::get_if<int>(&result))
				std::cout << "result: (int) " << *value << std::endl;
			else if (std::holds_alternative<vat::ast::SharedConstFnExp>(result))
				std::cout << "result: (fn)" << std::endl;
			else
				std::cout << "runtime error" << std::endl;
			ae.reset();
		}
		else
			res = 1;
	return res;
}
