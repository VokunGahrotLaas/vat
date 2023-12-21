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
	auto const exps = seq_exp.exps();
	auto it = exps.begin();
	if (it != exps.end()) (*it++)->accept(*this);
	while (it != exps.end())
	{
		os_ << ';';
		endline();
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
	auto const exps = fn_exp.args().exps();
	auto it = exps.begin();
	if (it != exps.end()) (*it++)->accept(*this);
	while (it != exps.end())
	{
		os_ << ", ";
		(*it++)->accept(*this);
	}
	++indent_;
	os_ << ") {";
	endline();
	fn_exp.body().accept(*this);
	--indent_;
	endline();
	os_ << '}';
}

void PrintVisitor::operator()(CallExp const& call_exp)
{
	call_exp.function().accept(*this);
	os_ << '(';
	auto const exps = call_exp.args().exps();
	auto it = exps.begin();
	if (it != exps.end()) (*it++)->accept(*this);
	while (it != exps.end())
	{
		os_ << ", ";
		(*it++)->accept(*this);
	}
	os_ << ')';
}

void PrintVisitor::operator()(LetExp const& let_exp)
{
	if (explicit_perens_) os_ << '(';
	os_ << "let ";
	let_exp.name().accept(*this);
	os_ << " = ";
	let_exp.value().accept(*this);
	if (explicit_perens_) os_ << ')';
}

void PrintVisitor::operator()(Bool const& bool_exp) { os_ << (bool_exp.value() ? "true" : "false"); }

void PrintVisitor::operator()(IfExp const& if_exp)
{
	os_ << "if ";
	if_exp.cond().accept(*this);
	++indent_;
	endline();
	os_ << "then ";
	if_exp.then_exp().accept(*this);
	if (!dynamic_cast<Unit const*>(&if_exp.else_exp()))
	{
		endline();
		os_ << "else ";
		if_exp.else_exp().accept(*this);
	}
	--indent_;
}

void PrintVisitor::operator()(Unit const&) { os_ << "()"; }

void PrintVisitor::endline() const { os_ << '\n' << std::string(indent_ * 4, ' '); }

} // namespace vat::ast
