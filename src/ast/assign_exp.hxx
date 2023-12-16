#pragma once

#include "assign_exp.hh"

namespace vat::ast
{

inline Name const& AssignExp::name() const { return *name_; }

inline Exp const& AssignExp::value() const { return *value_; }

inline Name& AssignExp::name() { return *name_; }

inline Exp& AssignExp::value() { return *value_; }

} // namespace vat::ast
