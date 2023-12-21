#pragma once

#include <vat/ast/exp.hh>
#include <vat/ast/name.hh>
#include "vat/ast/fwd.hh"

namespace vat::ast
{

class IfExp : public Exp
{
public:
	IfExp(Location const& location, SharedExp cond, SharedExp then_exp);
	IfExp(Location const& location, SharedExp cond, SharedExp then_exp, SharedExp else_exp);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Exp const& cond() const;
	Exp const& then_exp() const;
	Exp const& else_exp() const;
	Exp& cond();
	Exp& then_exp();
	Exp& else_exp();

private:
	SharedExp cond_;
	SharedExp then_exp_;
	SharedExp else_exp_;
};

} // namespace vat::ast

#include <vat/ast/if_exp.hxx>
