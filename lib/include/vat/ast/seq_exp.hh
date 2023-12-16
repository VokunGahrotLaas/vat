#pragma once

// STL
#include <memory>
#include <vector>

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class SeqExp : public Exp
{
public:
	SeqExp(Location const& location, std::vector<SharedExp> exps);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	std::vector<SharedConstExp> exps() const;
	std::vector<SharedExp> const& exps();

private:
	std::vector<SharedExp> exps_;
};

} // namespace vat::ast

#include <vat/ast/seq_exp.hxx>
