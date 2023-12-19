#pragma once

#include <vat/ast/ast.hh>

namespace vat::ast
{

inline Location const& Ast::location() const { return location_; }

inline void Ast::location(Location const& location) { location_ = location; }

template <std::derived_from<Ast> T>
std::shared_ptr<T> shared_from_ast(T& ast)
{
	return std::static_pointer_cast<T>(ast.shared_from_this());
}

template <std::derived_from<Ast> T>
std::shared_ptr<T const> shared_from_ast(T const& ast)
{
	return std::static_pointer_cast<T const>(ast.shared_from_this());
}

} // namespace vat::ast
