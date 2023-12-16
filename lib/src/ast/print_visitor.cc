#include <vat/ast/print_visitor.hh>

// vat
#include <vat/ast/all.hh>

namespace vat::ast
{

PrintVisitor::PrintVisitor(std::ostream& os, bool explicit_perens)
	: os_{ os }
	, explicit_perens_{ explicit_perens }
{}

void PrintVisitor::operator()(Ast const& ast) { ast.accept(*this); }

void PrintVisitor::operator()(Exp const& exp) { exp.accept(*this); }

void PrintVisitor::operator()(AssignExp const& assign_exp)
{
	if (explicit_perens_) os_ << '(';
	assign_exp.name().accept(*this);
	os_ << " := ";
	assign_exp.value().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(SeqExp const& seq_exp)
{
	std::vector<SharedConstExp> exps = seq_exp.exps();
	auto it = exps.begin();
	if (it != exps.end()) (*it++)->accept(*this);
	while (it != exps.end())
	{
		os_ << ' ';
		(*it++)->accept(*this);
	}
}

void PrintVisitor::operator()(Number const& number) { os_ << number.value(); }

void PrintVisitor::operator()(Name const& name) { os_ << name.value(); }

void PrintVisitor::operator()(UnaryOp const& unary_op)
{
	if (explicit_perens_) os_ << '(';
	os_ << unary_op.oper();
	unary_op.value().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(BinaryOp const& binary_op)
{
	if (explicit_perens_) os_ << '(';
	binary_op.lhs().accept(*this);
	os_ << ' ' << binary_op.oper() << ' ';
	binary_op.rhs().accept(*this);
	if (explicit_perens_) os_ << ')';
}

} // namespace vat::ast
