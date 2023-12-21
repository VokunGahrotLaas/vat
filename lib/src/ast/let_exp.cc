#include <vat/ast/let_exp.hh>

namespace vat::ast
{

LetExp::LetExp(Location const& location, SharedName name, SharedExp value)
	: Exp{ location }
	, name_{ std::move(name) }
	, value_{ std::move(value) }
{}

void LetExp::accept(Visitor& visitor) { visitor(*this); }

void LetExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
