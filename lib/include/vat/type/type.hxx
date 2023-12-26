#pragma once

#include <vat/type/type.hh>

namespace vat::type
{

inline bool Type::is_never() const { return false; }

inline Type const& Type::resolve() const { return *this; }

inline std::ostream& operator<<(std::ostream& os, Type const& type) { return type.print(os); }

} // namespace vat::type
