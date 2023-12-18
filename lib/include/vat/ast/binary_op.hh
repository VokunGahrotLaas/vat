#pragma once

// STL
#include <string_view>

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class BinaryOp : public Exp
{
public:
	enum Oper
	{
		Add,
		Sub,
		Mul,
		Div,
		Mod,
		Pow,
	};

	BinaryOp(Location const& location, Oper oper, SharedExp lhs, SharedExp rhs);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Oper oper() const;
	Exp const& lhs() const;
	Exp const& rhs() const;

	Exp& lhs();
	Exp& rhs();

	static constexpr std::string_view str(Oper oper);

private:
	Oper oper_;
	SharedExp lhs_;
	SharedExp rhs_;
};

} // namespace vat::ast

#include <vat/ast/binary_op.hxx>
