#include <vat/type/unit.hh>

namespace vat::type
{

template class Singleton<Unit>;

std::ostream& Unit::print(std::ostream& os) const { return os << "()"; }

} // namespace vat::type
