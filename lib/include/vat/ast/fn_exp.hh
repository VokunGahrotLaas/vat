#pragma once

#include <vat/ast/exp.hh>
#include <vat/ast/seq_exp.hh>

namespace vat::ast
{

class FnExp : public Exp
{
public:
	FnExp(Location const& location, SharedSeqExp args, SharedExp body);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	SeqExp const& args() const;
	Exp const& body() const;
	SeqExp& args();
	Exp& body();

private:
	SharedSeqExp args_;
	SharedExp body_;
};

} // namespace vat::ast

#include <vat/ast/fn_exp.hxx>
