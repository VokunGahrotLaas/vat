#pragma once

// STL
#include <type_traits>

// vat
#include "ast/fwd.hh"

namespace vat::ast
{

template <template <typename> typename Const>
class GenVisitor
{
public:
	virtual ~GenVisitor() = default;

	virtual void operator()(Const<Ast>&) = 0;
	virtual void operator()(Const<Exp>&) = 0;
	virtual void operator()(Const<AssignExp>&) = 0;
	virtual void operator()(Const<SeqExp>&) = 0;
	virtual void operator()(Const<Number>&) = 0;
	virtual void operator()(Const<Name>&) = 0;
	virtual void operator()(Const<UnaryOp>&) = 0;
	virtual void operator()(Const<BinaryOp>&) = 0;
};

using Visitor = GenVisitor<std::type_identity_t>;
using ConstVisitor = GenVisitor<std::add_const_t>;

} // namespace vat::ast
