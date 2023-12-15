#include "ast/binary_op.hh"

namespace vat::ast
{

BinaryOp::BinaryOp(Location const& location, std::string_view oper, std::shared_ptr<Exp> lhs, std::shared_ptr<Exp> rhs)
	: Exp{ location }
	, oper_{ oper }
	, lhs_{ std::move(lhs) }
	, rhs_{ std::move(rhs) }
{}

void BinaryOp::accept(Visitor& visitor) { visitor(*this); }

void BinaryOp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
