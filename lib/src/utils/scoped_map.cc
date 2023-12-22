#include <vat/utils/scoped_map.hh>

namespace vat::utils
{

bool ScopedMapDebug::debug_{ false };

void ScopedMapDebug::set(bool debug) { debug_ = debug; }

bool ScopedMapDebug::get() { return debug_; }

} // namespace vat::utils
