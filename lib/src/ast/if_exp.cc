#include <vat/ast/if_exp.hh>

// vat
#include <vat/ast/unit.hh>

namespace vat::ast
{

IfExp::IfExp(Location const& location, SharedExp cond, SharedExp then_exp, SharedExp else_exp)
	: Exp{ location }
	, cond_{ std::move(cond) }
	, then_exp_{ std::move(then_exp) }
	, else_exp_{ else_exp != nullptr ? std::move(else_exp) : std::make_shared<Unit>(location) }
{}

void IfExp::accept(Visitor& visitor) { visitor(*this); }

void IfExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
