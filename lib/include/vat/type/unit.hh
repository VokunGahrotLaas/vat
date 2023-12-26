#pragma once

// vat
#include <vat/type/singleton.hh>

namespace vat::type
{

class Unit;

extern template class Singleton<Unit>;

class Unit : public Singleton<Unit>
{
public:
	using super_type = Singleton<Unit>;
	friend super_type;

	std::ostream& print(std::ostream& os) const override;

private:
	Unit() = default;
};

} // namespace vat::type
