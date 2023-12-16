#include "ast/compute_visitor.hh"

// STL
#include <cmath>

// vat
#include "ast/all.hh"

namespace vat::ast
{

void ComputeVisitor::operator()(Ast const& ast) { ast.accept(*this); }

void ComputeVisitor::operator()(Exp const& exp) { exp.accept(*this); }

void ComputeVisitor::operator()(AssignExp const& assign_exp)
{
	assign_exp.value().accept(*this);
	if (result_) variables_[assign_exp.name().value()] = *result_;
}

void ComputeVisitor::operator()(SeqExp const& seq_exp)
{
	for (SharedConstExp& exp: seq_exp.exps())
		exp->accept(*this);
}

void ComputeVisitor::operator()(Number const& number) { result_ = number.value(); }

void ComputeVisitor::operator()(Name const& name) { result_ = variable(name.value()); }

void ComputeVisitor::operator()(UnaryOp const& unary_op)
{
	unary_op.value().accept(*this);
	if (result_ && unary_op.oper() == "-")
		result_ = -*result_;
	else if (unary_op.oper() != "+")
		result_.reset();
}

void ComputeVisitor::operator()(BinaryOp const& binary_op)
{
	binary_op.lhs().accept(*this);
	if (!result_) return;
	std::optional<int> lhs = *result_;
	binary_op.rhs().accept(*this);
	if (!result_) return;
	if (binary_op.oper() == "+")
		result_ = *lhs + *result_;
	else if (binary_op.oper() == "-")
		result_ = *lhs - *result_;
	else if (binary_op.oper() == "*")
		result_ = *lhs * *result_;
	else if (binary_op.oper() == "/" && *result_ != 0)
		result_ = *lhs / *result_;
	else if (binary_op.oper() == "**")
		result_ = std::pow(*lhs, *result_);
	else
		result_.reset();
}

} // namespace vat::ast
