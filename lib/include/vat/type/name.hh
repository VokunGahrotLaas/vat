#pragma once

// vat
#include <vat/type/type.hh>

namespace vat::type
{

class Name : public Type
{
public:
	Name(std::string_view name, SharedConstType type);

	std::string const& name() const;
	SharedConstType type() const;

	bool is_chain() const;

	bool is_never() const override;
	Type const& resolve() const override;

	bool assignable_from(Type const& type) const override;
	bool operator==(Type const& type) const override;
	std::ostream& print(std::ostream& os) const override;

private:
	std::string name_;
	SharedConstType type_;
};

} // namespace vat::type

#include <vat/type/name.hxx>
