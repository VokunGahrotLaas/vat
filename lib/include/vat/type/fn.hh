#pragma once

// STL
#include <vector>

// vat
#include <vat/type/type.hh>

namespace vat::type
{

class Fn : public Type
{
public:
	using super_type = Type;

	Fn(SharedConstType return_type, std::vector<SharedConstType> args);

	bool assignable_from(Type const& type) const override;
	bool operator==(Type const& type) const override;

	std::ostream& print(std::ostream& os) const override;

	SharedConstType return_type() const;
	std::vector<SharedConstType> const& args() const;

private:
	SharedConstType return_type_;
	std::vector<SharedConstType> args_;
};

} // namespace vat::type

#include <vat/type/fn.hxx>
