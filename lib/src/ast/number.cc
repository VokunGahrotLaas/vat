#include <vat/ast/number.hh>

namespace vat::ast
{

Number::Number(Location const& location, int value)
	: Exp{ location }
	, value_{ value }
{}

void Number::accept(Visitor& visitor) { visitor(*this); }

void Number::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
