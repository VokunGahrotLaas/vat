#include "ast/name.hh"

namespace vat::ast
{

Name::Name(Location const& location, std::string_view value)
	: Exp{ location }
	, value_{ value }
{}

void Name::accept(Visitor& visitor) { visitor(*this); }

void Name::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
