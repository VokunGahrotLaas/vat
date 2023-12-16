// STL
#include <iostream>

// vat
#include <vat/ast/compute_visitor.hh>
#include <vat/ast/print_visitor.hh>
#include <vat/parser/parser.hh>

int main(int argc, char** argv)
{
	int res = 0;
	vat::parser::Parser parser;
	vat::ast::PrintVisitor pv{ std::cout, true };
	vat::ast::ComputeVisitor cv;
	for (int i = 1; i < argc; ++i)
		if (argv[i] == std::string("-p"))
			parser.set_trace_parsing(true);
		else if (argv[i] == std::string("-s"))
			parser.set_trace_scanning(true);
		else if (auto const ast = parser.parse(argv[i]))
		{
			pv(*ast);
			std::cout << std::endl;
			cv(*ast);
			auto result = cv.result();
			if (result)
				std::cout << "result: " << *result << std::endl;
			else
				std::cout << "runtime error" << std::endl;
			cv.reset_variables();
		}
		else
			res = 1;
	return res;
}
