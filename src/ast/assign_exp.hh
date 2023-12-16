#pragma once

#include "ast/exp.hh"
#include "ast/name.hh"

namespace vat::ast
{

class AssignExp : public Exp
{
public:
	AssignExp(Location const& location, SharedName name, SharedExp value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Name const& name() const;
	Exp const& value() const;
	Name& name();
	Exp& value();

private:
	SharedName name_;
	SharedExp value_;
};

} // namespace vat::ast

#include "assign_exp.hxx"
