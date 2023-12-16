#pragma once

#include <vat/ast/unary_op.hh>

namespace vat::ast
{

inline std::string_view UnaryOp::oper() const { return oper_; }

inline Exp& UnaryOp::value() const { return *value_; }

} // namespace vat::ast
