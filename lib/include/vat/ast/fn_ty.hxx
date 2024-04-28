#pragma once

#include <vat/ast/fn_ty.hh>

namespace vat::ast
{

inline SeqExp const& FnTy::args() const { return *args_; }

inline Exp const& FnTy::return_type() const { return *return_type_; }

inline SeqExp& FnTy::args() { return *args_; }

inline Exp& FnTy::return_type() { return *return_type_; }

inline type::SharedConstType FnTy::value() const { return value_; }

inline void FnTy::value(type::SharedConstType v) { value_ = v; }

} // namespace vat::ast
