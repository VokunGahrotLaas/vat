#pragma once

// STL
#include <unordered_map>

// vat
#include <vat/ast/all.hh>
#include <vat/ast/default_visitor.hh>
#include <vat/type/all.hh>
#include <vat/utils/error.hh>

#include "vat/eval/ast.hh"

namespace vat::type
{

class TypeChecker : private ast::DefaultVisitor
{
public:
	using super_type = ast::DefaultVisitor;

	TypeChecker(utils::ErrorManager& em);

	void type(ast::Ast& ast);
	void reset();

protected:
	bool check_assignable(ast::Location const& location, Type const& to, Type const& from);
	bool check_same(ast::Location const& location, Type const& expected, Type const& got);

	void operator()(ast::AssignExp& assign_exp) override;
	void operator()(ast::SeqExp& seq_exp) override;
	void operator()(ast::Number& number) override;
	void operator()(ast::Name& name) override;
	void operator()(ast::UnaryOp& unary_op) override;
	void operator()(ast::BinaryOp& binary_op) override;
	void operator()(ast::FnExp& fn_exp) override;
	void operator()(ast::CallExp& call_exp) override;
	void operator()(ast::LetExp& let_exp) override;
	void operator()(ast::Bool& bool_exp) override;
	void operator()(ast::IfExp& if_exp) override;
	void operator()(ast::BlockExp& block_exp) override;

	static std::unordered_map<std::string, SharedConstType> const static_vars_;
	utils::ErrorManager& error_;
	eval::AstEvaluator constexpr_eval_{ error_ };
	bool recurse_body_{ true };
};

} // namespace vat::type

#include <vat/type/type_checker.hxx>
