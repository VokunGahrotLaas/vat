#pragma once

// vat
#include <vat/type/singleton.hh>

namespace vat::type
{

class Bool;

extern template class Singleton<Bool>;

class Bool : public Singleton<Bool>
{
public:
	using super_type = Singleton<Bool>;
	friend super_type;

	std::ostream& print(std::ostream& os) const override;

private:
	Bool() = default;
};

} // namespace vat::type
