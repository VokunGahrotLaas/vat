#pragma once

#include <vat/ast/call_exp.hh>

namespace vat::ast
{

inline Name const& CallExp::name() const { return *name_; }

inline SeqExp const& CallExp::args() const { return *args_; }

inline Name& CallExp::name() { return *name_; }

inline SeqExp& CallExp::args() { return *args_; }

} // namespace vat::ast
