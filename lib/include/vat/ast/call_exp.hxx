#pragma once

#include <vat/ast/call_exp.hh>

namespace vat::ast
{

inline Exp const& CallExp::function() const { return *function_; }

inline SeqExp const& CallExp::args() const { return *args_; }

inline Exp& CallExp::function() { return *function_; }

inline SeqExp& CallExp::args() { return *args_; }

} // namespace vat::ast
