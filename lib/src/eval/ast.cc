#include <variant>
#include <vat/eval/ast.hh>

// STL
#include <cmath>
#include <stdexcept>

#include "vat/ast/fwd.hh"
#include "vat/utils/variant.hh"

namespace vat::eval
{

void AstEvaluator::operator()(ast::Ast const& ast) { ast.accept(*this); }

void AstEvaluator::operator()(ast::Exp const& exp) { exp.accept(*this); }

void AstEvaluator::operator()(ast::AssignExp const& assign_exp)
{
	auto opt = variables_.get(assign_exp.name().value());
	if (!opt) throw std::runtime_error{ "AssignExp: not such variable " + assign_exp.name().value() };
	exp_type& old_value = opt->get();
	assign_exp.value().accept(*this);
	if (old_value.index() != result_.index())
		throw std::runtime_error{ "AssignExp: wrong type for variable " + assign_exp.name().value() };
	old_value = result_;
}

void AstEvaluator::operator()(ast::SeqExp const& seq_exp)
{
	for (ast::SharedConstExp const& exp: seq_exp.exps())
		exp->accept(*this);
}

void AstEvaluator::operator()(ast::Number const& number) { result_ = number.value(); }

void AstEvaluator::operator()(ast::Name const& name)
{
	auto opt = variable(name.value());
	if (!opt) throw std::runtime_error{ "Name: no such variable " + name.value() };
	result_ = *opt;
}

void AstEvaluator::operator()(ast::UnaryOp const& unary_op)
{
	unary_op.value().accept(*this);
	int* result = std::get_if<int>(&result_);
	if (!result) throw std::runtime_error{ "UnaryOp: non int operand" };
	switch (unary_op.oper())
	{
	case ast::UnaryOp::Pos: break;
	case ast::UnaryOp::Neg: *result = -*result; break;
	default: throw std::runtime_error{ "UnaryOp: missing case for UnaryOp" };
	}
}

void AstEvaluator::operator()(ast::BinaryOp const& binary_op)
{
	binary_op.lhs().accept(*this);
	int* result = std::get_if<int>(&result_);
	if (!result) throw std::runtime_error{ "BinaryOp: non int lhs" };
	int lhs = *result;
	binary_op.rhs().accept(*this);
	result = std::get_if<int>(&result_);
	if (!result) throw std::runtime_error{ "BinaryOp: non int rhs" };
	int rhs = *result;
	switch (binary_op.oper())
	{
	case ast::BinaryOp::Add: *result = lhs + rhs; break;
	case ast::BinaryOp::Sub: *result = lhs - rhs; break;
	case ast::BinaryOp::Mul: *result = lhs * rhs; break;
	case ast::BinaryOp::Div: *result = lhs / rhs; break;
	case ast::BinaryOp::Mod: *result = lhs % rhs; break;
	case ast::BinaryOp::Pow: *result = std::pow(lhs, rhs); break;
	default: throw std::runtime_error{ "BinaryOp: missing case for UnaryOp" };
	}
}

void AstEvaluator::operator()(ast::FnExp const& fn_exp) { result_ = shared_from_ast(fn_exp); }

void AstEvaluator::operator()(ast::CallExp const& call_exp)
{
	call_exp.function().accept(*this);
	ast::SharedConstFnExp* ast = std::get_if<ast::SharedConstFnExp>(&result_);
	if (!ast) throw std::runtime_error{ "CallExp: not a function" };
	ast::FnExp const& fn_exp = **ast;
	if (fn_exp.args().size() != call_exp.args().size()) throw std::runtime_error{ "CallExp: invalid number of args" };
	{
		auto call_scope = variables_.call_scope();
		for (std::size_t i = 0; i < call_exp.args().size(); ++i)
		{
			call_exp.args()[i].accept(*this);
			variables_.insert_or_assign(static_cast<ast::Name const&>(fn_exp.args()[i]).value(), result_);
		}
		fn_exp.body().accept(*this);
	}
}

void AstEvaluator::operator()(ast::LetExp const& let_exp)
{
	let_exp.value().accept(*this);
	variables_.insert_or_assign(let_exp.name().value(), result_);
}

void AstEvaluator::operator()(ast::Bool const& bool_exp) { result_ = bool_exp.value(); }

void AstEvaluator::operator()(ast::IfExp const& if_exp)
{
	if_exp.cond().accept(*this);
	bool* cond = std::get_if<bool>(&result_);
	if (!cond) throw std::runtime_error{ "IfExp: cond is not a bool" };
	(*cond ? if_exp.then_exp() : if_exp.else_exp()).accept(*this);
}

void AstEvaluator::operator()(ast::Unit const&) { result_ = {}; }

auto AstEvaluator::eval(input_type input) -> exp_type
{
	try
	{
		operator()(input);
	}
	catch (std::exception const& e)
	{
		std::cerr << "AstEvaluator::" << e.what() << std::endl;
		return {};
	}
	return result_;
}

void AstEvaluator::reset()
{
	result_ = {};
	variables_.clear();
}

void AstEvaluator::print_exp(std::ostream& os, exp_type exp)
{
	std::visit(utils::overloads{ [&os](int v) { os << "(int) " << v; }, [&os](std::monostate) { os << "(unit) ()"; },
								 [&os](bool v) { os << "(bool) " << (v ? "true" : "false"); },
								 [&os](ast::SharedConstFnExp v) { os << "(fn) " << v->location(); } },
			   exp);
}

} // namespace vat::eval
