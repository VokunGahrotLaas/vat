#include <vat/type/never.hh>

namespace vat::type
{

template class Singleton<Never>;

std::ostream& Never::print(std::ostream& os) const { return os << "!"; }

} // namespace vat::type
