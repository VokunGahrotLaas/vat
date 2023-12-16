#pragma once

#include "seq_exp.hh"

namespace vat::ast
{

inline std::vector<SharedConstExp> SeqExp::exps() const { return { exps_.cbegin(), exps_.cend() }; }

inline std::vector<SharedExp> const& SeqExp::exps() { return exps_; }

} // namespace vat::ast
