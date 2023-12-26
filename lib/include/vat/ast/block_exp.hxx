#pragma once

#include <vat/ast/block_exp.hh>

namespace vat::ast
{

inline Exp const& BlockExp::exp() const { return *exp_; }

inline Exp& BlockExp::exp() { return *exp_; }

} // namespace vat::ast
