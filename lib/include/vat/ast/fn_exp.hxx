#pragma once

#include <vat/ast/fn_exp.hh>

namespace vat::ast
{

inline SeqExp const& FnExp::args() const { return *args_; }

inline Exp const& FnExp::body() const { return *body_; }

inline Exp const& FnExp::return_type() const { return *return_type_; }

inline SeqExp& FnExp::args() { return *args_; }

inline Exp& FnExp::body() { return *body_; }

inline Exp& FnExp::return_type() { return *return_type_; }

} // namespace vat::ast
