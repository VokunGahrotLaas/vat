#pragma once

#include <vat/ast/exp.hh>
#include <vat/ast/seq_exp.hh>
#include <vat/type/fwd.hh>

namespace vat::ast
{

class FnTy : public Exp
{
public:
	FnTy(Location const& location, SharedSeqExp args, SharedExp return_type);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	SeqExp const& args() const;
	Exp const& return_type() const;
	SeqExp& args();
	Exp& return_type();

	type::SharedConstType value() const;
	void value(type::SharedConstType v);

private:
	SharedSeqExp args_;
	SharedExp return_type_;
	type::SharedConstType value_;
};

} // namespace vat::ast

#include <vat/ast/fn_ty.hxx>
