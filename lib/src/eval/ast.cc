#include <vat/eval/ast.hh>

// STL
#include <cmath>
#include <memory>
#include <stdexcept>

namespace vat::eval
{

void AstEvaluator::operator()(ast::Ast const& ast) { ast.accept(*this); }

void AstEvaluator::operator()(ast::Exp const& exp) { exp.accept(*this); }

void AstEvaluator::operator()(ast::AssignExp const& assign_exp)
{
	assign_exp.value().accept(*this);
	variables_.put(assign_exp.name().value(), result_);
}

void AstEvaluator::operator()(ast::SeqExp const& seq_exp)
{
	for (ast::SharedConstExp const& exp: seq_exp)
		exp->accept(*this);
}

void AstEvaluator::operator()(ast::Number const& number) { result_ = number.value(); }

void AstEvaluator::operator()(ast::Name const& name) { result_ = variable(name.value()).value(); }

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

void AstEvaluator::operator()(ast::FnExp const& fn_exp)
{
	result_ = std::static_pointer_cast<ast::FnExp const>(fn_exp.shared_from_this());
}

void AstEvaluator::operator()(ast::CallExp const& call_exp)
{
	std::optional<exp_type> func = variable(call_exp.name().value());
	if (!func) throw std::runtime_error{ "CallExp: no such variable " + call_exp.name().value() };
	ast::SharedConstFnExp* ast = std::get_if<ast::SharedConstFnExp>(&*func);
	if (!ast) throw std::runtime_error{ "CallExp: no such function " + call_exp.name().value() };
	ast::FnExp const& fn_exp = **ast;
	if (fn_exp.args().size() != call_exp.args().size())
		throw std::runtime_error{ "CallExp: invalid number of args " + call_exp.name().value() };
	{
		auto scope = variables_.scope();
		for (std::size_t i = 0; i < call_exp.args().size(); ++i)
		{
			call_exp.args()[i].accept(*this);
			variables_.put(static_cast<ast::Name const&>(fn_exp.args()[i]).value(), result_);
		}
		(*ast)->body().accept(*this);
	}
}

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

} // namespace vat::eval
