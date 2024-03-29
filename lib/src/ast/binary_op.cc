#include <vat/ast/binary_op.hh>

namespace vat::ast
{

BinaryOp::BinaryOp(Location const& location, Oper oper, SharedExp lhs, SharedExp rhs)
	: Exp{ location }
	, oper_{ oper }
	, lhs_{ std::move(lhs) }
	, rhs_{ std::move(rhs) }
{}

void BinaryOp::accept(Visitor& visitor) { visitor(*this); }

void BinaryOp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
