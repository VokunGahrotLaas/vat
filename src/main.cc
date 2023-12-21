// STL
#include <iostream>
#include <span>
#include <string_view>
#include <variant>

// vat
#include <vat/ast/print_visitor.hh>
#include <vat/eval/ast.hh>
#include <vat/parser/parser.hh>
#include <vat/utils/tasks.hh>

// vat prog
// #include "args/args.hh"

namespace vat
{

struct MainCtx
{
	parser::Parser parser;
	ast::SharedAst ast;
};

int main(std::span<std::string_view const> args);

MainCtx& main_print(MainCtx& ctx);
MainCtx& main_eval(MainCtx& ctx);

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
	MainCtx ctx;
	for (auto arg: args.subspan(1))
		if (arg == "-p")
			ctx.parser.set_trace_parsing(true);
		else if (arg == "-s")
			ctx.parser.set_trace_scanning(true);
		else if ((ctx.ast = ctx.parser.parse(arg)))
		{
			main_print(ctx);
			main_eval(ctx);
		}
		else
			res = 1;
	return res;
}

MainCtx& main_print(MainCtx& ctx)
{
	ast::PrintVisitor{ std::cout, true }(*ctx.ast);
	std::cout << std::endl;
	return ctx;
}

MainCtx& main_eval(MainCtx& ctx)
{
	eval::ast_exp::exp_type exp = eval::AstEvaluator{}.eval(*ctx.ast);
	std::visit(utils::overloads{ [](int value) { std::cout << "result: (int) " << value << std::endl; },
								 [](ast::SharedConstFnExp) { std::cout << "result: (fn)" << std::endl; },
								 [](auto&&) { std::cout << "runtime error" << std::endl; } },
			   exp);
	return ctx;
}

} // namespace vat
