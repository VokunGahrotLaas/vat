#pragma once

// STL
#include <memory>
#include <string_view>

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class UnaryOp : public Exp
{
public:
	enum Oper
	{
		Pos,
		Neg,
	};

	UnaryOp(Location const& location, Oper oper, SharedExp value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Oper oper() const;
	Exp const& value() const;
	Exp& value();

	static constexpr std::string_view str(Oper oper);

private:
	Oper oper_;
	SharedExp value_;
};

} // namespace vat::ast

#include <vat/ast/unary_op.hxx>
