#pragma once

// STL
#include <vector>

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class SeqExp : public Exp
{
public:
	using iterator = std::vector<SharedExp>::const_iterator;

	SeqExp(Location const& location, std::vector<SharedExp> exps);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	iterator begin();
	iterator end();

	std::vector<SharedConstExp> exps() const;

	void push_back(SharedExp exp);
	std::size_t size() const;
	Exp const& operator[](std::size_t index) const;
	Exp& operator[](std::size_t index);

private:
	std::vector<SharedExp> exps_;
};

} // namespace vat::ast

#include <vat/ast/seq_exp.hxx>
