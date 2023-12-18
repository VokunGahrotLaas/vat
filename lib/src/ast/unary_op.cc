#include <vat/ast/unary_op.hh>

namespace vat::ast
{

UnaryOp::UnaryOp(Location const& location, Oper oper, SharedExp value)
	: Exp{ location }
	, oper_{ oper }
	, value_{ std::move(value) }
{}

void UnaryOp::accept(Visitor& visitor) { visitor(*this); }

void UnaryOp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
