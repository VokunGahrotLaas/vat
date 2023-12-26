#include <vat/type/number.hh>

namespace vat::type
{

template class Singleton<Number>;

std::ostream& Number::print(std::ostream& os) const { return os << "int"; }

} // namespace vat::type
