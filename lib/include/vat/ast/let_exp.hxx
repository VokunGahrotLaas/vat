#pragma once

#include <vat/ast/let_exp.hh>

// vat
#include <vat/ast/name.hh>

namespace vat::ast
{

inline Name const& LetExp::name() const { return *name_; }

inline Exp const& LetExp::type_name() const { return *type_name_; }

inline Exp const& LetExp::value() const { return *value_; }

inline Name& LetExp::name() { return *name_; }

inline Exp& LetExp::type_name() { return *type_name_; }

inline Exp& LetExp::value() { return *value_; }

inline bool LetExp::has_value() const { return value_ != nullptr; }

inline bool LetExp::has_type_name() const { return type_name_ != nullptr; }

inline bool LetExp::is_rec() const { return is_rec_; }

} // namespace vat::ast
