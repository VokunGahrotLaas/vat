#pragma once

#include <vat/utils/utils.hh>

namespace vat::utils
{

[[noreturn]]
inline void unreachable() noexcept
{
	__builtin_unreachable();
}

} // namespace vat::utils
