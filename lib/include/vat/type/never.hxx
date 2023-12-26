#pragma once

#include <vat/type/never.hh>

namespace vat::type
{

inline bool Never::is_never() const { return true; }

inline bool Never::assignable_from(Type const&) const { return false; }

} // namespace vat::type
