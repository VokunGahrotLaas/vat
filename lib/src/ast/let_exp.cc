#include <vat/ast/let_exp.hh>

namespace vat::ast
{

LetExp::LetExp(Location const& location, bool is_rec, SharedName name, SharedExp type_name, SharedExp value)
	: Exp{ location }
	, name_{ std::move(name) }
	, type_name_{ std::move(type_name) }
	, value_{ std::move(value) }
	, is_rec_{ is_rec }
{}

void LetExp::accept(Visitor& visitor) { visitor(*this); }

void LetExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
