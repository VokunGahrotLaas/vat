#include <vat/ast/bool.hh>

namespace vat::ast
{

Bool::Bool(Location const& location, bool value)
	: Exp{ location }
	, value_{ value }
{}

void Bool::accept(Visitor& visitor) { visitor(*this); }

void Bool::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
