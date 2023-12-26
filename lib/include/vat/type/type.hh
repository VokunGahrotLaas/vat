#pragma once

// vat
#include <vat/type/fwd.hh>

namespace vat::type
{

class Type
{
public:
	virtual bool is_never() const;
	virtual Type const& resolve() const;

	virtual bool assignable_from(Type const& type) const = 0;
	virtual bool operator==(Type const& type) const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;

	bool operator!=(Type const&) const = default;
};

std::ostream& operator<<(std::ostream& os, Type const& type);

} // namespace vat::type

#include <vat/type/type.hxx>
