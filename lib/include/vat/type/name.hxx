#pragma once

#include <vat/type/name.hh>

// STL
#include <stdexcept>
#include <unordered_set>

namespace vat::type
{

inline std::string const& Name::name() const { return name_; }

inline SharedConstType Name::type() const { return type_; }

inline bool Name::is_chain() const
{
	std::unordered_set<Name const*> names_;
	for (Name const* name = this; name != nullptr; name = dynamic_cast<Name const*>(&*name->type_))
		if (!names_.insert(name).second) return true;
	return false;
}

inline bool Name::is_never() const
{
	if (is_chain()) throw std::runtime_error{ "Name::is_never: is a chain" };
	return type_->is_never();
}

inline Type const& Name::resolve() const
{
	if (is_chain()) throw std::runtime_error{ "Name::resolve: is a chain" };
	return type_->resolve();
}

inline bool Name::assignable_from(Type const& type) const
{
	if (is_chain()) throw std::runtime_error{ "Name::assignable_from: is a chain" };
	return type_->assignable_from(type);
}

inline bool Name::operator==(Type const& type) const
{
	if (is_chain()) throw std::runtime_error{ "Name::operator==: is a chain" };
	return *type_ == type;
}

inline std::ostream& Name::print(std::ostream& os) const
{
	os << name_ << " = ";
	if (is_chain())
		os << "[[chain]]";
	else if (type_ != nullptr)
		os << *type_;
	else
		os << "[[nullptr]]";
	return os;
}

} // namespace vat::type
