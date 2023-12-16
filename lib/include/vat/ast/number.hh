#pragma once

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class Number : public Exp
{
public:
	Number(Location const& location, int value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	int value() const;

private:
	int value_;
};

} // namespace vat::ast

#include <vat/ast/number.hxx>
