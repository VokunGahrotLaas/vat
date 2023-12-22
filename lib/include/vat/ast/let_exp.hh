#pragma once

#include <vat/ast/exp.hh>

namespace vat::ast
{

class LetExp : public Exp
{
public:
	LetExp(Location const& location, SharedName name, SharedExp value = nullptr);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Name const& name() const;
	Exp const& value() const;
	Name& name();
	Exp& value();
	bool has_value() const;

private:
	SharedName name_;
	SharedExp value_;
};

} // namespace vat::ast

#include <vat/ast/let_exp.hxx>
