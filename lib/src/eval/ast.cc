#include <vat/eval/ast.hh>

// STL
#include <cmath>
#include <stdexcept>

// vat
#include <vat/utils/utils.hh>
#include <vat/utils/variant.hh>

namespace vat::eval
{

AstEvaluator::AstEvaluator(utils::ErrorManager& em)
	: error_{ em }
{}

void AstEvaluator::operator()(ast::Ast const& ast) { ast.accept(*this); }

void AstEvaluator::operator()(ast::Exp const& exp) { exp.accept(*this); }

void AstEvaluator::operator()(ast::AssignExp const& assign_exp)
{
	exp_type& old_value = vars_[assign_exp.name().let_exp()];
	assign_exp.value().accept(*this);
	if (old_value.index() != result_.index())
	{
		error_.error(utils::ErrorType::Evaluation, assign_exp.location()) << "assignation from a different type";
		return;
	}
	old_value = result_;
}

void AstEvaluator::operator()(ast::SeqExp const& seq_exp)
{
	for (ast::SharedConstExp const& exp: seq_exp.exps())
		exp->accept(*this);
}

void AstEvaluator::operator()(ast::Number const& number) { result_ = number.value(); }

void AstEvaluator::operator()(ast::Name const& name) { result_ = vars_[name.let_exp()]; }

void AstEvaluator::operator()(ast::UnaryOp const& unary_op)
{
	unary_op.value().accept(*this);
	int* result = std::get_if<int>(&result_);
	if (!result)
	{
		error_.error(utils::ErrorType::Evaluation, unary_op.value().location())
			<< "unary operation operand is not an integer";
		return;
	}
	switch (unary_op.oper())
	{
	case ast::UnaryOp::Pos: break;
	case ast::UnaryOp::Neg: *result = -*result; break;
	default: utils::unreachable();
	}
}

void AstEvaluator::operator()(ast::BinaryOp const& binary_op)
{
	binary_op.lhs().accept(*this);
	int* result = std::get_if<int>(&result_);
	if (!result)
	{
		error_.error(utils::ErrorType::Evaluation, binary_op.lhs().location())
			<< "binary operation lhs is not an integer";
		return;
	}
	int lhs = *result;
	binary_op.rhs().accept(*this);
	result = std::get_if<int>(&result_);
	if (!result)
	{
		error_.error(utils::ErrorType::Evaluation, binary_op.rhs().location())
			<< "binary operation rhs is not an integer";
		return;
	}
	int rhs = *result;
	switch (binary_op.oper())
	{
	case ast::BinaryOp::Add: *result = lhs + rhs; break;
	case ast::BinaryOp::Sub: *result = lhs - rhs; break;
	case ast::BinaryOp::Mul: *result = lhs * rhs; break;
	case ast::BinaryOp::Div: *result = lhs / rhs; break;
	case ast::BinaryOp::Mod: *result = lhs % rhs; break;
	case ast::BinaryOp::Pow: *result = std::pow(lhs, rhs); break;
	case ast::BinaryOp::Eq: result_ = lhs == rhs; break;
	case ast::BinaryOp::Ne: result_ = lhs != rhs; break;
	case ast::BinaryOp::Lt: result_ = lhs < rhs; break;
	case ast::BinaryOp::Le: result_ = lhs <= rhs; break;
	case ast::BinaryOp::Gt: result_ = lhs > rhs; break;
	case ast::BinaryOp::Ge: result_ = lhs >= rhs; break;
	default: utils::unreachable();
	}
}

void AstEvaluator::operator()(ast::FnExp const& fn_exp) { result_ = shared_from_ast(fn_exp); }

void AstEvaluator::operator()(ast::CallExp const& call_exp)
{
	call_exp.function().accept(*this);
	ast::SharedConstFnExp* ast = std::get_if<ast::SharedConstFnExp>(&result_);
	if (!ast)
	{
		error_.error(utils::ErrorType::Evaluation, call_exp.function().location()) << "calling non function";
		return;
	}
	ast::FnExp const& fn_exp = **ast;
	if (fn_exp.args().size() != call_exp.args().size())
	{
		error_.error(utils::ErrorType::Evaluation, call_exp.location())
			<< "invalid number of args, got " << call_exp.args().size() << " expected " << fn_exp.args().size();
		return;
	}
	std::vector<exp_type> args(call_exp.args().size());
	for (std::size_t i = 0; i < args.size(); ++i)
	{
		call_exp.args()[i].accept(*this);
		args[i] = result_;
	}
	auto call_scope = vars_.call_scope();
	for (std::size_t i = 0; i < args.size(); ++i)
		vars_.insert_or_assign(ast::shared_from_ast(static_cast<ast::LetExp const&>(fn_exp.args()[i])), args[i]);
	fn_exp.body().accept(*this);
}

void AstEvaluator::operator()(ast::LetExp const& let_exp)
{
	let_exp.value().accept(*this);
	vars_.insert_or_assign(ast::shared_from_ast(let_exp), result_);
}

void AstEvaluator::operator()(ast::Bool const& bool_exp) { result_ = bool_exp.value(); }

void AstEvaluator::operator()(ast::IfExp const& if_exp)
{
	if_exp.cond().accept(*this);
	bool* cond = std::get_if<bool>(&result_);
	if (!cond)
	{
		error_.error(utils::ErrorType::Evaluation, if_exp.cond().location()) << "if condition if not a bool";
		return;
	}
	(*cond ? if_exp.then_exp() : if_exp.else_exp()).accept(*this);
}

void AstEvaluator::operator()(ast::Unit const&) { result_ = {}; }

auto AstEvaluator::eval(input_type input) -> exp_type
{
	operator()(input);
	return result_;
}

void AstEvaluator::reset()
{
	result_ = {};
	vars_.clear();
}

void AstEvaluator::print_exp(std::ostream& os, exp_type exp)
{
	std::visit(utils::overloads{ [&os](int v) { os << "(int) " << v; }, [&os](std::monostate) { os << "(unit) ()"; },
								 [&os](bool v) { os << "(bool) " << (v ? "true" : "false"); },
								 [&os](ast::SharedConstFnExp v) { os << "(fn) " << v->location(); } },
			   exp);
}

} // namespace vat::eval
