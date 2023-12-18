#include <vat/ast/fn_exp.hh>

namespace vat::ast
{

FnExp::FnExp(Location const& location, SharedSeqExp args, SharedExp body)
	: Exp{ location }
	, args_{ std::move(args) }
	, body_{ std::move(body) }
{}

void FnExp::accept(Visitor& visitor) { visitor(*this); }

void FnExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
