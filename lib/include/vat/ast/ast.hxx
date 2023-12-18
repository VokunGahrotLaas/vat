#pragma once

#include <vat/ast/ast.hh>

namespace vat::ast
{

inline Location const& Ast::location() const { return location_; }

inline void Ast::location(Location const& location) { location_ = location; }

} // namespace vat::ast
