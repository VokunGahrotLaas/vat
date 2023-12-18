#include <vat/ast/call_exp.hh>

namespace vat::ast
{

CallExp::CallExp(Location const& location, SharedName name, SharedSeqExp args)
	: Exp{ location }
	, name_{ std::move(name) }
	, args_{ std::move(args) }
{}

void CallExp::accept(Visitor& visitor) { visitor(*this); }

void CallExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
