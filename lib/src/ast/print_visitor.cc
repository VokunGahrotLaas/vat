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
	os_ << " = ";
	assign_exp.value().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(SeqExp const& seq_exp)
{
	auto it = seq_exp.begin();
	if (it != seq_exp.end()) (*it++)->accept(*this);
	while (it != seq_exp.end())
	{
		os_ << ";\n" << std::string(indent_, '\t');
		(*it++)->accept(*this);
	}
}

void PrintVisitor::operator()(Number const& number) { os_ << number.value(); }

void PrintVisitor::operator()(Name const& name) { os_ << name.value(); }

void PrintVisitor::operator()(UnaryOp const& unary_op)
{
	if (explicit_perens_) os_ << '(';
	os_ << UnaryOp::str(unary_op.oper());
	unary_op.value().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(BinaryOp const& binary_op)
{
	if (explicit_perens_) os_ << '(';
	binary_op.lhs().accept(*this);
	os_ << ' ' << BinaryOp::str(binary_op.oper()) << ' ';
	binary_op.rhs().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(FnExp const& fn_exp)
{
	os_ << "fn (";
	auto it = fn_exp.args().begin();
	if (it != fn_exp.args().end()) (*it++)->accept(*this);
	while (it != fn_exp.args().end())
	{
		os_ << ", ";
		(*it++)->accept(*this);
	}
	++indent_;
	os_ << ") {\n" << std::string(indent_, '\t');
	fn_exp.body().accept(*this);
	--indent_;
	os_ << "\n" << std::string(indent_, '\t') << "}";
}

void PrintVisitor::operator()(CallExp const& call_exp)
{
	call_exp.name().accept(*this);
	os_ << '(';
	auto it = call_exp.args().begin();
	if (it != call_exp.args().end()) (*it++)->accept(*this);
	while (it != call_exp.args().end())
	{
		os_ << ", ";
		(*it++)->accept(*this);
	}
	os_ << ')';
}

} // namespace vat::ast
