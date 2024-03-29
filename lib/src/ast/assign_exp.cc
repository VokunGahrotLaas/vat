#include <vat/ast/assign_exp.hh>

namespace vat::ast
{

AssignExp::AssignExp(Location const& location, SharedName name, SharedExp value)
	: Exp{ location }
	, name_{ std::move(name) }
	, value_{ std::move(value) }
{}

void AssignExp::accept(Visitor& visitor) { visitor(*this); }

void AssignExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
