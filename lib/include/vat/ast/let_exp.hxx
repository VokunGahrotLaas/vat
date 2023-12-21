#pragma once

#include <vat/ast/let_exp.hh>

namespace vat::ast
{

inline Name const& LetExp::name() const { return *name_; }

inline Exp const& LetExp::value() const { return *value_; }

inline Name& LetExp::name() { return *name_; }

inline Exp& LetExp::value() { return *value_; }

} // namespace vat::ast
