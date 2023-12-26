#pragma once

#include <vat/type/fn.hh>

namespace vat::type
{

inline bool Fn::assignable_from(Type const& type) const
{
	Type const& rtype = type.resolve();
	if (rtype.is_never()) return true;
	auto fn = dynamic_cast<Fn const*>(&rtype);
	if (fn == nullptr || fn->args_.size() != args_.size()) return false;
	for (std::size_t i = 0; i < args_.size(); ++i)
		if (!args_[i]->assignable_from(*fn->args_[i])) return false;
	return true;
}

inline bool Fn::operator==(Type const& type) const
{
	Type const& rtype = type.resolve();
	if (rtype.is_never()) return true;
	auto fn = dynamic_cast<Fn const*>(&rtype);
	if (fn == nullptr || fn->args_.size() != args_.size()) return false;
	for (std::size_t i = 0; i < args_.size(); ++i)
		if (*args_[i] != *fn->args_[i]) return false;
	return true;
}

inline SharedConstType Fn::return_type() const { return return_type_; }

inline std::vector<SharedConstType> const& Fn::args() const { return args_; }

} // namespace vat::type
