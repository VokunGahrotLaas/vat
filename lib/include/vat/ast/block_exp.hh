#pragma once

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class BlockExp : public Exp
{
public:
	BlockExp(Location const& location, SharedExp exp);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	Exp const& exp() const;
	Exp& exp();

private:
	SharedExp exp_;
};

} // namespace vat::ast

#include <vat/ast/block_exp.hxx>
