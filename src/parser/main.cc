// STL
#include <iostream>

// vat
#include "ast/print_visitor.hh"
#include "parser/parser.hh"

int main(int argc, char** argv)
{
	int res = 0;
	vat::parser::Parser parser;
	vat::ast::PrintVisitor pv{ std::cout, true };
	for (int i = 1; i < argc; ++i)
		if (argv[i] == std::string("-p"))
			parser.set_trace_parsing(true);
		else if (argv[i] == std::string("-s"))
			parser.set_trace_scanning(true);
		else if (auto const result = parser.parse(argv[i]))
		{
			pv(*result);
			std::cout << std::endl << "result: " << 0 << std::endl;
		}
		else
			res = 1;
	return res;
}
