#pragma once

#include <vat/ast/exp.hh>

namespace vat::ast
{

class LetExp : public Exp
{
public:
	LetExp(Location const& location, bool is_rec, SharedName name, SharedExp type_name, SharedExp value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Name const& name() const;
	Exp const& type_name() const;
	Exp const& value() const;
	Name& name();
	Exp& type_name();
	Exp& value();
	bool has_value() const;
	bool has_type_name() const;
	bool is_rec() const;

private:
	SharedName name_;
	SharedExp type_name_;
	SharedExp value_;
	bool is_rec_;
};

} // namespace vat::ast

#include <vat/ast/let_exp.hxx>
