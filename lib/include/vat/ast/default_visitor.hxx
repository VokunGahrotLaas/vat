#pragma once

#include <vat/ast/default_visitor.hh>

// vat
#include <vat/ast/all.hh>

namespace vat::ast
{

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<Ast>& ast)
{
	ast.accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<Exp>& exp)
{
	exp.accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<AssignExp>& assign_exp)
{
	assign_exp.name().accept(*this);
	assign_exp.value().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<SeqExp>& seq_exp)
{
	for (size_t i = 0; i < seq_exp.size(); ++i)
		seq_exp[i].accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<Number>&)
{}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<Name>&)
{}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<UnaryOp>& unary_op)
{
	unary_op.value().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<BinaryOp>& binary_op)
{
	binary_op.lhs().accept(*this);
	binary_op.rhs().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<FnExp>& fn_exp)
{
	fn_exp.args().accept(*this);
	fn_exp.return_type().accept(*this);
	fn_exp.body().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<FnTy>& fn_ty)
{
	fn_ty.args().accept(*this);
	fn_ty.return_type().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<CallExp>& call_exp)
{
	call_exp.function().accept(*this);
	call_exp.args().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<LetExp>& let_exp)
{
	let_exp.name().accept(*this);
	if (let_exp.has_type_name()) let_exp.type_name().accept(*this);
	if (let_exp.has_value()) let_exp.value().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<Bool>&)
{}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<IfExp>& if_exp)
{
	if_exp.cond().accept(*this);
	if_exp.then_exp().accept(*this);
	if_exp.else_exp().accept(*this);
}

template <template <typename> typename Const>
void GenDefaultVisitor<Const>::operator()(Const<BlockExp>& block_exp)
{
	block_exp.exp().accept(*this);
}

} // namespace vat::ast
