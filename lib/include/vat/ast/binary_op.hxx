#pragma once

#include <vat/ast/binary_op.hh>

namespace vat::ast
{

inline std::string_view BinaryOp::oper() const { return oper_; }

inline Exp const& BinaryOp::lhs() const { return *lhs_; }

inline Exp const& BinaryOp::rhs() const { return *rhs_; }

inline Exp& BinaryOp::lhs() { return *lhs_; }

inline Exp& BinaryOp::rhs() { return *rhs_; }

} // namespace vat::ast
