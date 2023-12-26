#include <vat/ast/block_exp.hh>

namespace vat::ast
{

BlockExp::BlockExp(Location const& location, SharedExp exp)
	: Exp{ location }
	, exp_{ std::move(exp) }
{}

void BlockExp::accept(Visitor& visitor) { visitor(*this); }

void BlockExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
