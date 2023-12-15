#pragma once

// STL
#include <memory>
#include <string_view>

// vat
#include "ast/exp.hh"

namespace vat::ast
{

class BinaryOp : public Exp
{
public:
	BinaryOp(Location const& location, std::string_view oper, std::shared_ptr<Exp> lhs, std::shared_ptr<Exp> rhs);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	std::string_view oper() const;
	Exp const& lhs() const;
	Exp const& rhs() const;

	Exp& lhs();
	Exp& rhs();

private:
	std::string oper_;
	std::shared_ptr<Exp> lhs_;
	std::shared_ptr<Exp> rhs_;
};

} // namespace vat::ast

#include "ast/binary_op.hxx"
