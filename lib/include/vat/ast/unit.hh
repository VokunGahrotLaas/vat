#pragma once

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class Unit : public Exp
{
public:
	Unit(Location const& location);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;
};

} // namespace vat::ast
