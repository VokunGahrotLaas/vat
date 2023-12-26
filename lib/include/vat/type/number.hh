#pragma once

// vat
#include <vat/type/singleton.hh>

namespace vat::type
{

class Number;

extern template class Singleton<Number>;

class Number : public Singleton<Number>
{
public:
	using super_type = Singleton<Number>;
	friend super_type;

	std::ostream& print(std::ostream& os) const override;

private:
	Number() = default;
};

} // namespace vat::type
