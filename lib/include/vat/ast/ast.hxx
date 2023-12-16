#pragma once

#include <vat/ast/ast.hh>

namespace vat::ast
{

inline Location const& Ast::location() const { return location_; }

} // namespace vat::ast
