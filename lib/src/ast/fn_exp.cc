#include <vat/ast/fn_exp.hh>

// vat
#include <vat/ast/name.hh>

namespace vat::ast
{

FnExp::FnExp(Location const& location, SharedSeqExp args, SharedExp body, SharedExp return_type)
	: Exp{ location }
	, args_{ std::move(args) }
	, body_{ std::move(body) }
	, return_type_{ return_type ? std::move(return_type) : std::make_shared<Name>(location, "unit") }
{}

void FnExp::accept(Visitor& visitor) { visitor(*this); }

void FnExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
