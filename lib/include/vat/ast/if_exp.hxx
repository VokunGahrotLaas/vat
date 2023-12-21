#pragma once

#include <vat/ast/if_exp.hh>

namespace vat::ast
{

inline Exp const& IfExp::cond() const { return *cond_; }

inline Exp const& IfExp::then_exp() const { return *then_exp_; }

inline Exp const& IfExp::else_exp() const { return *else_exp_; }

inline Exp& IfExp::cond() { return *cond_; }

inline Exp& IfExp::then_exp() { return *then_exp_; }

inline Exp& IfExp::else_exp() { return *else_exp_; }

} // namespace vat::ast
