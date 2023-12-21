#pragma once

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class Bool : public Exp
{
public:
	Bool(Location const& location, bool value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	bool value() const;

private:
	bool value_;
};

} // namespace vat::ast

#include <vat/ast/bool.hxx>
