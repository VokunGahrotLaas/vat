// STL
#include <iostream>
#include <span>
#include <string_view>

// vat
#include <vat/ast/print_visitor.hh>
#include <vat/bind/binder.hh>
#include <vat/eval/ast.hh>
#include <vat/parse/parser.hh>
#include <vat/type/type_checker.hh>
#include <vat/utils/error.hh>
#include <vat/utils/scoped_map.hh>
#include <vat/utils/tasks.hh>

// vat prog
// #include "args/args.hh"

namespace vat
{

struct MainCtx
{
	std::string_view filename{};
	ast::SharedAst ast{};
	utils::ErrorManager em{};
	parse::Parser parser{ em };
	bool trace_binding{ false };
	bool trace_types{ false };
};

int main(std::span<std::string_view const> args);
int main_arg(MainCtx& ctx, std::string_view arg, std::optional<std::string_view> next);

MainCtx& main_parse(MainCtx& ctx);
MainCtx& main_print(MainCtx& ctx);
MainCtx& main_bind(MainCtx& ctx);
MainCtx& main_type(MainCtx& ctx);
MainCtx& main_eval(MainCtx& ctx);

} // namespace vat

int main(int argc, char const* const* argv)
{
	std::vector<std::string_view> const args{ argv, argv + argc };
	return vat::main(args);
}

namespace vat
{

int main(std::span<std::string_view const> args)
{
	MainCtx ctx;
	for (auto it = args.begin() + 1; it != args.end(); ++it)
	{
		auto next = it + 1 != args.end() ? std::optional<std::string_view>{ *(it + 1) } : std::nullopt;
		int res = main_arg(ctx, *it, next);
		if (res != 0) return res;
	}
	if (ctx.filename.empty())
	{
		std::cerr << "no input file given" << std::endl;
		return 1;
	}
	main_parse(ctx);
	if (ctx.em)
	{
		std::cerr << ctx.em;
		return utils::enum_code(ctx.em.type());
	}
	main_bind(ctx);
	if (ctx.em)
	{
		std::cerr << ctx.em;
		return utils::enum_code(ctx.em.type());
	}
	main_type(ctx);
	if (ctx.em)
	{
		std::cerr << ctx.em;
		return utils::enum_code(ctx.em.type());
	}
	main_print(ctx);
	main_eval(ctx);
	if (ctx.em)
	{
		std::cerr << ctx.em;
		return utils::enum_code(ctx.em.type());
	}
	return 0;
}

int main_arg(MainCtx& ctx, std::string_view arg, std::optional<std::string_view>)
{
	if (arg == "-p")
		ctx.parser.set_trace_parsing(true);
	else if (arg == "-s")
		ctx.parser.set_trace_scanning(true);
	else if (arg == "-b")
		ctx.trace_binding = true;
	else if (arg == "-t")
		ctx.trace_types = true;
	else if (arg == "-m")
		utils::ScopedMapDebug::set(true);
	else
		ctx.filename = arg;
	return 0;
}

MainCtx& main_parse(MainCtx& ctx)
{
	ctx.ast = ctx.parser.parse(ctx.filename);
	return ctx;
}

MainCtx& main_print(MainCtx& ctx)
{
	ast::PrintVisitor{ std::cout, true, ctx.trace_binding, ctx.trace_types }(*ctx.ast);
	std::cout << std::endl;
	return ctx;
}

MainCtx& main_bind(MainCtx& ctx)
{
	bind::Binder{ ctx.em }.bind(*ctx.ast);
	return ctx;
}

MainCtx& main_type(MainCtx& ctx)
{
	type::TypeChecker{ ctx.em }.type(*ctx.ast);
	return ctx;
}

MainCtx& main_eval(MainCtx& ctx)
{
	eval::ast_exp::exp_type exp = eval::AstEvaluator{ ctx.em }.eval(*ctx.ast);
	if (ctx.em) return ctx;
	eval::AstEvaluator::print_exp(std::cout, exp);
	std::cout << std::endl;
	return ctx;
}

} // namespace vat
