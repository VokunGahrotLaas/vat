#include "ast/print_visitor.hh"

// vat
#include "ast/all.hh"

namespace vat::ast
{

PrintVisitor::PrintVisitor(std::ostream& os, bool explicit_perens)
	: os_{ os }
	, explicit_perens_{ explicit_perens }
{}

void PrintVisitor::operator()(Ast const& ast) { ast.accept(*this); }

void PrintVisitor::operator()(Exp const& exp) { exp.accept(*this); }

void PrintVisitor::operator()(Number const& number) { os_ << number.value(); }

void PrintVisitor::operator()(Name const& name) { os_ << name.value(); }

void PrintVisitor::operator()(UnaryOp const& un_op)
{
	if (explicit_perens_) os_ << '(';
	os_ << un_op.oper();
	un_op.value().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(BinaryOp const& bin_op)
{
	if (explicit_perens_) os_ << '(';
	bin_op.lhs().accept(*this);
	os_ << ' ' << bin_op.oper() << ' ';
	bin_op.rhs().accept(*this);
	if (explicit_perens_) os_ << ')';
}

} // namespace vat::ast
