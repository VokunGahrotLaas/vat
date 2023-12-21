#pragma once

#include <vat/ast/exp.hh>
#include <vat/ast/name.hh>

namespace vat::ast
{

class CallExp : public Exp
{
public:
	CallExp(Location const& location, SharedExp function, SharedSeqExp args);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Exp const& function() const;
	SeqExp const& args() const;
	Exp& function();
	SeqExp& args();

private:
	SharedExp function_;
	SharedSeqExp args_;
};

} // namespace vat::ast

#include <vat/ast/call_exp.hxx>
