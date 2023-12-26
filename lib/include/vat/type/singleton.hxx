#pragma once

#include <vat/type/singleton.hh>

namespace vat::type
{

template <typename T>
std::shared_ptr<T const> Singleton<T>::instance()
{
	static std::shared_ptr<T const> instance_ = std::shared_ptr<T const>{ new T const {} };
	return instance_;
}

template <typename T>
bool Singleton<T>::operator==(Type const& type) const
{
	Type const& rtype = type.resolve();
	return is_never() || rtype.is_never() || this == &rtype;
}

template <typename T>
bool Singleton<T>::assignable_from(Type const& type) const
{
	return *this == type;
}

} // namespace vat::type
