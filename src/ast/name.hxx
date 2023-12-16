#pragma once

#include "ast/name.hh"

namespace vat::ast
{

inline std::string const& Name::value() const { return value_; }

} // namespace vat::ast
