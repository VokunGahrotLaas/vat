#include <vat/type/type_type.hh>

namespace vat::type
{

template class Singleton<TypeType>;

std::ostream& TypeType::print(std::ostream& os) const { return os << "type"; }

} // namespace vat::type
