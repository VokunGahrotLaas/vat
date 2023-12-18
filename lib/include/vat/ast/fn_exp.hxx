#pragma once

#include <vat/ast/fn_exp.hh>

namespace vat::ast
{

inline SeqExp const& FnExp::args() const { return *args_; }

inline Exp const& FnExp::body() const { return *body_; }

inline SeqExp& FnExp::args() { return *args_; }

inline Exp& FnExp::body() { return *body_; }

} // namespace vat::ast
