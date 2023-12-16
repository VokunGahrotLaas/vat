#include "seq_exp.hh"

namespace vat::ast
{

SeqExp::SeqExp(Location const& location, std::vector<SharedExp> exps)
	: Exp{ location }
	, exps_{ std::move(exps) }
{}

void SeqExp::accept(Visitor& visitor) { visitor(*this); }

void SeqExp::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
