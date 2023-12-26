#pragma once

// vat
#include <vat/type/singleton.hh>

namespace vat::type
{

class TypeType;

extern template class Singleton<TypeType>;

class TypeType : public Singleton<TypeType>
{
public:
	using super_type = Singleton<TypeType>;
	friend super_type;

	std::ostream& print(std::ostream& os) const override;

private:
	TypeType() = default;
};

} // namespace vat::type

#include <vat/type/never.hxx>
