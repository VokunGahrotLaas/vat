#include <vat/type/bool.hh>

namespace vat::type
{

template class Singleton<Bool>;

std::ostream& Bool::print(std::ostream& os) const { return os << "bool"; }

} // namespace vat::type
