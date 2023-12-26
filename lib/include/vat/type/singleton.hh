#pragma once

// STL
#include <memory>

// vat
#include <vat/type/type.hh>

namespace vat::type
{

template <typename T>
class Singleton : public Type
{
public:
	using super_type = Type;

	Singleton(Singleton const&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton const&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	static std::shared_ptr<T const> instance();

	bool assignable_from(Type const& type) const override;
	bool operator==(Type const& type) const override;

protected:
	Singleton() = default;
};

} // namespace vat::type

#include <vat/type/singleton.hxx>
