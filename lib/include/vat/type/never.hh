#pragma once

// vat
#include <vat/type/singleton.hh>

namespace vat::type
{

class Never;

extern template class Singleton<Never>;

class Never : public Singleton<Never>
{
public:
	using super_type = Singleton<Never>;
	friend super_type;

	bool is_never() const override;

	bool assignable_from(Type const& type) const override;

	std::ostream& print(std::ostream& os) const override;

private:
	Never() = default;
};

} // namespace vat::type

#include <vat/type/never.hxx>
