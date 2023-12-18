#pragma once

// STL
#include <iostream>

// vat
#include <vat/ast/visitor.hh>

namespace vat::ast
{

class PrintVisitor : public ConstVisitor
{
public:
	PrintVisitor(std::ostream& os = std::cout, bool explicit_perens = false);

	void operator()(Ast const& ast) override;
	void operator()(Exp const& exp) override;
	void operator()(AssignExp const& assign_exp) override;
	void operator()(SeqExp const& seq_exp) override;
	void operator()(Number const& number) override;
	void operator()(Name const& name) override;
	void operator()(UnaryOp const& unary_op) override;
	void operator()(BinaryOp const& binary_op) override;
	void operator()(FnExp const& fn_exp) override;
	void operator()(CallExp const& call_exp) override;

private:
	std::ostream& os_;
	bool explicit_perens_;
	std::size_t indent_{};
};

} // namespace vat::ast
