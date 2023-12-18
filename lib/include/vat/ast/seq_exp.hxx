#pragma once

#include <vat/ast/seq_exp.hh>

namespace vat::ast
{

inline auto SeqExp::begin() -> iterator { return exps_.cbegin(); }

inline auto SeqExp::begin() const -> const_iterator { return cbegin(); }

inline auto SeqExp::cbegin() const -> const_iterator
{
	return reinterpret_cast<std::vector<SharedConstExp> const&>(exps_).cbegin();
}

inline auto SeqExp::end() -> iterator { return exps_.cend(); }

inline auto SeqExp::end() const -> const_iterator
{
	return cend();
	;
}

inline auto SeqExp::cend() const -> const_iterator
{
	return reinterpret_cast<std::vector<SharedConstExp> const&>(exps_).cend();
}

inline void SeqExp::push_back(SharedExp exp) { exps_.push_back(std::move(exp)); }

inline std::size_t SeqExp::size() const { return exps_.size(); }

inline Exp const& SeqExp::operator[](std::size_t index) const { return *exps_[index]; }

inline Exp& SeqExp::operator[](std::size_t index) { return *exps_[index]; }

} // namespace vat::ast
