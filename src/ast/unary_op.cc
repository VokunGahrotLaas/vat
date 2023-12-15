#include "ast/unary_op.hh"

namespace vat::ast
{

UnaryOp::UnaryOp(Location const& location, std::string_view oper, std::shared_ptr<Exp> value)
	: Exp{ location }
	, oper_{ oper }
	, value_{ std::move(value) }
{}

void UnaryOp::accept(Visitor& visitor) { visitor(*this); }

void UnaryOp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
