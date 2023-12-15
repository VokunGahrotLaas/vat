#pragma once

#include "ast/name.hh"

namespace vat::ast
{

inline std::string_view Name::value() const { return value_; }

} // namespace vat::ast
