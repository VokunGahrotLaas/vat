#pragma once

#include <vat/utils/scoped_map.hh>

// STL
#include <functional>
#include <stdexcept>

// debug
#include <cstdint>
#include <iostream>
#include <memory>
#include <variant>
#include <vat/ast/all.hh>

namespace vat::utils
{

template <typename Key, typename Value>
ScopedMap<Key, Value>::Scope::~Scope()
{
	scoped_map_.scope_end(level_);
}

template <typename Key, typename Value>
ScopedMap<Key, Value>::Scope::Scope(ScopedMap& scoped_map, std::size_t level)
	: scoped_map_{ scoped_map }
	, level_{ level }
{}

template <typename Key, typename Value>
ScopedMap<Key, Value>::~ScopedMap()
{
	scope_end(0);
}

template <typename Key, typename Value>
auto ScopedMap<Key, Value>::scope() -> Scope
{
	if (scopes_.back().map.empty())
		++scopes_.back().overriden;
	else
		scopes_.emplace_back(map_type{}, level());
	return Scope{ *this, level() };
}

template <typename Key, typename Value>
auto ScopedMap<Key, Value>::call_scope() -> Scope
{
	if (scopes_.back().map.empty() && scopes_.back().prev == 0)
		++scopes_.back().overriden;
	else
		scopes_.emplace_back(map_type{}, 0);
	return Scope{ *this, level() };
}

template <typename Key, typename Value>
void ScopedMap<Key, Value>::clear()
{
	if (level() != 0) throw std::runtime_error{ "ScopedMap::clear: scope error" };
	scopes_.clear();
}

template <typename Key, typename Value>
void ScopedMap<Key, Value>::insert_or_assign(Key const& key, Value const& value)
{
	scopes_.back().map.insert_or_assign(key, value);
}

template <typename Key, typename Value>
bool ScopedMap<Key, Value>::insert(Key const& key, Value const& value)
{
	return scopes_.back().map.insert(key, value).second;
}

template <typename Key, typename Value>
std::optional<std::reference_wrapper<Value const>> ScopedMap<Key, Value>::get(Key const& key) const
{
	for (std::size_t idx = scopes_.size() - 1;; idx = scopes_[idx].prev)
	{
		if (auto const it = scopes_[idx].map.find(key); it != scopes_[idx].map.end()) return it->second;
		if (idx == scopes_[idx].prev) break;
	}
	return {};
}

template <typename Key, typename Value>
std::optional<std::reference_wrapper<Value>> ScopedMap<Key, Value>::get(Key const& key)
{
	for (std::size_t idx = scopes_.size() - 1;; idx = scopes_[idx].prev)
	{
		if (auto const it = scopes_[idx].map.find(key); it != scopes_[idx].map.end()) return it->second;
		if (idx == scopes_[idx].prev) break;
	}
	return {};
}

template <typename Key, typename Value>
Value const& ScopedMap<Key, Value>::operator[](Key const& key) const
{
	return get(key)->get();
}

template <typename Key, typename Value>
Value& ScopedMap<Key, Value>::operator[](Key const& key)
{
	return get(key)->get();
}

inline std::ostream& operator<<(std::ostream& os, ast::SharedConstLetExp const& ptr)
{
	return os << ptr->name().value();
}

inline std::ostream& operator<<(std::ostream& os, ast::SharedConstFnExp const& ptr) { return os << ptr->location(); }

inline std::ostream& operator<<(std::ostream& os, std::monostate const&) { return os << "()"; }

template <typename Arg, typename... Args>
std::ostream& operator<<(std::ostream& os, std::variant<Arg, Args...> const& variant)
{
	std::visit([&os](auto const& val) { os << std::boolalpha << val; }, variant);
	return os;
}

template <typename Key, typename Value>
void ScopedMap<Key, Value>::scope_end(std::size_t level)
{
	if (this->level() != level) throw std::runtime_error{ "ScopedMap::scope_end: scope error" };
	std::cout << "Scope end: " << this->level() << " | " << scopes_.back().prev << " | " << scopes_.back().overriden
			  << std::endl;
	for (auto& [key, val]: scopes_.back().map)
		std::cout << "* " << key << ": " << val << std::endl;
	if (scopes_.back().overriden > 0)
	{
		scopes_.back().map.clear();
		--scopes_.back().overriden;
	}
	else
		scopes_.pop_back();
}

template <typename Key, typename Value>
std::size_t ScopedMap<Key, Value>::level() const
{
	return scopes_.size() - 1;
}

} // namespace vat::utils
