#pragma once

#include <vat/ast/seq_exp.hh>

namespace vat::ast
{

inline auto SeqExp::begin() -> iterator { return exps_.cbegin(); }

inline auto SeqExp::end() -> iterator { return exps_.cend(); }

inline std::vector<SharedConstExp> SeqExp::exps() const { return { exps_.cbegin(), exps_.cend() }; }

inline void SeqExp::push_back(SharedExp exp) { exps_.push_back(std::move(exp)); }

inline std::size_t SeqExp::size() const { return exps_.size(); }

inline Exp const& SeqExp::operator[](std::size_t index) const { return *exps_[index]; }

inline Exp& SeqExp::operator[](std::size_t index) { return *exps_[index]; }

} // namespace vat::ast
