#pragma once

// vat
#include <vat/ast/visitor.hh>

namespace vat::ast
{

template <template <typename> typename Const>
class GenDefaultVisitor : public GenVisitor<Const>
{
public:
	using super_type = GenVisitor<Const>;

	~GenDefaultVisitor() override = default;

	void operator()(Const<Ast>& ast) override;
	void operator()(Const<Exp>& exp) override;
	void operator()(Const<AssignExp>& assign_exp) override;
	void operator()(Const<SeqExp>& seq_exp) override;
	void operator()(Const<Number>& number) override;
	void operator()(Const<Name>& name) override;
	void operator()(Const<UnaryOp>& unary_op) override;
	void operator()(Const<BinaryOp>& binary_op) override;
	void operator()(Const<FnExp>& fn_exp) override;
	void operator()(Const<FnTy>& fn_ty) override;
	void operator()(Const<CallExp>& call_exp) override;
	void operator()(Const<LetExp>& let_exp) override;
	void operator()(Const<Bool>& bool_exp) override;
	void operator()(Const<IfExp>& if_exp) override;
	void operator()(Const<BlockExp>& block_exp) override;
};

using DefaultVisitor = GenDefaultVisitor<std::type_identity_t>;
using DefaultConstVisitor = GenDefaultVisitor<std::add_const_t>;

} // namespace vat::ast

#include <vat/ast/default_visitor.hxx>
