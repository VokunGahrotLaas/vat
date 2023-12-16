#pragma once

#include <vat/ast/number.hh>

namespace vat::ast
{

inline int Number::value() const { return value_; }

} // namespace vat::ast
