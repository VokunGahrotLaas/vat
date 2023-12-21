#pragma once

// STL
#include <type_traits>

// vat
#include <vat/ast/fwd.hh>

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
	virtual void operator()(Const<FnExp>&) = 0;
	virtual void operator()(Const<CallExp>&) = 0;
	virtual void operator()(Const<LetExp>&) = 0;
	virtual void operator()(Const<Bool>&) = 0;
	virtual void operator()(Const<IfExp>&) = 0;
	virtual void operator()(Const<Unit>&) = 0;
};

using Visitor = GenVisitor<std::type_identity_t>;
using ConstVisitor = GenVisitor<std::add_const_t>;

} // namespace vat::ast
