#include <vat/type/name.hh>

namespace vat::type
{

Name::Name(std::string_view name, SharedConstType type)
	: name_{ name }
	, type_{ std::move(type) }
{}

} // namespace vat::type
