#pragma once

#include <vat/ast/exp.hh>

namespace vat::ast
{

inline type::SharedConstType Exp::type() const { return type_; }

inline void Exp::type(type::SharedConstType type) { type_ = type; }

} // namespace vat::ast
