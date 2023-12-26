#pragma once

#include <vat/ast/exp.hh>
#include <vat/ast/seq_exp.hh>

namespace vat::ast
{

class FnExp : public Exp
{
public:
	FnExp(Location const& location, SharedSeqExp args, SharedExp body, SharedExp return_type = nullptr);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	SeqExp const& args() const;
	Exp const& body() const;
	Exp const& return_type() const;
	SeqExp& args();
	Exp& body();
	Exp& return_type();

private:
	SharedSeqExp args_;
	SharedExp body_;
	SharedExp return_type_;
};

} // namespace vat::ast

#include <vat/ast/fn_exp.hxx>
