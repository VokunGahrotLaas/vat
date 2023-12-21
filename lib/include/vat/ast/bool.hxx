#pragma once

#include <vat/ast/bool.hh>

namespace vat::ast
{

inline bool Bool::value() const { return value_; }

} // namespace vat::ast
