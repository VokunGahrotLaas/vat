#pragma once

#include <vat/ast/exp.hh>
#include <vat/ast/name.hh>

namespace vat::ast
{

class CallExp : public Exp
{
public:
	CallExp(Location const& location, SharedName name, SharedSeqExp args);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Name const& name() const;
	SeqExp const& args() const;
	Name& name();
	SeqExp& args();

private:
	SharedName name_;
	SharedSeqExp args_;
};

} // namespace vat::ast

#include <vat/ast/call_exp.hxx>
