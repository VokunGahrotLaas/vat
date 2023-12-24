#pragma once

// STL
#include <optional>
#include <unordered_map>
#include <variant>

// vat
#include <vat/ast/all.hh>
#include <vat/ast/visitor.hh>
#include <vat/eval/evaluator.hh>
#include <vat/utils/error.hh>
#include <vat/utils/scoped_map.hh>

namespace vat::eval
{

namespace ast_exp
{

using unit_type = std::monostate;

using bool_type = bool;

using int_type = int;

using function_type = ast::SharedConstFnExp;

using exp_type = std::variant<unit_type, bool_type, int_type, function_type>;

}; // namespace ast_exp

class AstEvaluator
	: public Evaluator<ast::Ast const&, ast_exp::exp_type>
	, public ast::ConstVisitor
{
public:
	AstEvaluator(utils::ErrorManager& error);

	void operator()(ast::Ast const& ast) override;
	void operator()(ast::Exp const& exp) override;
	void operator()(ast::AssignExp const& assign_exp) override;
	void operator()(ast::SeqExp const& seq_exp) override;
	void operator()(ast::Number const& number) override;
	void operator()(ast::Name const& name) override;
	void operator()(ast::UnaryOp const& unary_op) override;
	void operator()(ast::BinaryOp const& binary_op) override;
	void operator()(ast::FnExp const& fn_exp) override;
	void operator()(ast::CallExp const& call_exp) override;
	void operator()(ast::LetExp const& let_exp) override;
	void operator()(ast::Bool const& bool_exp) override;
	void operator()(ast::IfExp const& if_exp) override;
	void operator()(ast::Unit const& unit) override;

	exp_type eval(input_type input) override;
	void reset() override;

	static void print_exp(std::ostream& os, exp_type exp);

private:
	utils::ErrorManager& error_;
	utils::ScopedMap<ast::SharedConstLetExp, exp_type> vars_{};
	exp_type result_{};
};

} // namespace vat::eval
