#include <vat/ast/unit.hh>

namespace vat::ast
{

Unit::Unit(Location const& location)
	: Exp{ location }
{}

void Unit::accept(Visitor& visitor) { visitor(*this); }

void Unit::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
