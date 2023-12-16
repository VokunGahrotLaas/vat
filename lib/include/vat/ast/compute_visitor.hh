#pragma once

// STL
#include <iostream>
#include <optional>
#include <unordered_map>

// vat
#include <vat/ast/visitor.hh>

namespace vat::ast
{

class ComputeVisitor : public ConstVisitor
{
public:
	void operator()(Ast const& ast) override;
	void operator()(Exp const& exp) override;
	void operator()(AssignExp const& assign_exp) override;
	void operator()(SeqExp const& seq_exp) override;
	void operator()(Number const& number) override;
	void operator()(Name const& name) override;
	void operator()(UnaryOp const& unary_op) override;
	void operator()(BinaryOp const& binary_op) override;

	std::optional<int> result() const;
	std::optional<int> variable(std::string const& name);
	void reset_variables();

private:
	std::optional<int> result_{};
	std::unordered_map<std::string, int> variables_;
};

} // namespace vat::ast

#include <vat/ast/compute_visitor.hxx>
