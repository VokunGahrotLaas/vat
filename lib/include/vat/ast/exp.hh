#pragma once

// vat
#include <vat/ast/ast.hh>
#include <vat/type/fwd.hh>

namespace vat::ast
{

class Exp : public Ast
{
public:
	using Ast::Ast;

	type::SharedConstType type() const;
	void type(type::SharedConstType type);

private:
	type::SharedConstType type_{};
};

} // namespace vat::ast

#include <vat/ast/exp.hxx>
