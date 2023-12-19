// STL
#include <iostream>
#include <span>
#include <string_view>

// vat
#include <variant>
#include <vat/ast/print_visitor.hh>
#include <vat/eval/ast.hh>
#include <vat/parser/parser.hh>

// vat prog
#include "tasks/tasks.hh"
// #include "args/args.hh"

namespace vat
{

int main(std::span<std::string_view const> args);

} // namespace vat

int main(int argc, char const* const* argv)
{
	std::vector<std::string_view> const args{ argv, argv + argc };
	vat::main(args);
}

namespace vat
{

int main(std::span<std::string_view const> args)
{
	int res = 0;
	parser::Parser parser;
	ast::PrintVisitor pv{ std::cout, true };
	eval::AstEvaluator ae;
	for (auto arg: args.subspan(1))
		if (arg == "-p")
			parser.set_trace_parsing(true);
		else if (arg == "-s")
			parser.set_trace_scanning(true);
		else if (ast::SharedAst ast = parser.parse(arg))
		{
			auto task = make_task<ast::Ast&>([&pv](ast::Ast& ast) -> ast::Ast& {
							pv(ast);
							std::cout << std::endl;
							return ast;
						})
							.and_then(make_task<ast::Ast&>([&ae](ast::Ast& ast) { return ae.eval(ast); }))
							.and_then(make_task<eval::ast_exp::exp_type>([](eval::ast_exp::exp_type exp) {
								std::visit(utils::overloads{
											   [](int value) { std::cout << "result: (int) " << value << std::endl; },
											   [](ast::SharedConstFnExp) { std::cout << "result: (fn)" << std::endl; },
											   [](auto&&) { std::cout << "runtime error" << std::endl; } },
										   exp);
							}));
			ae.reset();
		}
		else
			res = 1;
	return res;
}

} // namespace vat
