#include <vat/ast/call_exp.hh>

namespace vat::ast
{

CallExp::CallExp(Location const& location, SharedExp function, SharedSeqExp args)
	: Exp{ location }
	, function_{ std::move(function) }
	, args_{ std::move(args) }
{}

void CallExp::accept(Visitor& visitor) { visitor(*this); }

void CallExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
