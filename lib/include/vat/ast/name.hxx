#pragma once

#include <vat/ast/name.hh>

// vat
#include <vat/ast/let_exp.hh>

namespace vat::ast
{

inline std::string const& Name::value() const { return value_; }

inline SharedConstLetExp Name::let_exp() const { return let_exp_; }

inline void Name::let_exp(SharedConstLetExp let_exp) { let_exp_ = let_exp; }

} // namespace vat::ast
