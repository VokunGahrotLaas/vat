#include <vat/ast/fn_ty.hh>

// vat
#include <vat/ast/name.hh>

namespace vat::ast
{

FnTy::FnTy(Location const& location, SharedSeqExp args, SharedExp return_type)
	: Exp{ location }
	, args_{ std::move(args) }
	, return_type_{ return_type ? std::move(return_type) : std::make_shared<Name>(location, "unit") }
	, value_{nullptr}
{}

void FnTy::accept(Visitor& visitor) { visitor(*this); }

void FnTy::accept(ConstVisitor& visitor) const { visitor(*this); }

} // namespace vat::ast
