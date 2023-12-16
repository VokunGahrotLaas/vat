#pragma once

// STL
#include <memory>
#include <string_view>

// vat
#include "ast/exp.hh"

namespace vat::ast
{

class UnaryOp : public Exp
{
public:
	UnaryOp(Location const& location, std::string_view oper, SharedExp value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	std::string_view oper() const;
	Exp& value() const;

private:
	std::string oper_;
	SharedExp value_;
};

} // namespace vat::ast

#include "ast/unary_op.hxx"
