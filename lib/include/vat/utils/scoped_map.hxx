#pragma once

#include <stdexcept>
#include <vat/utils/scoped_map.hh>

namespace vat::utils
{

template <typename Key, typename Value>
ScopedMap<Key, Value>::Scope::Scope(ScopedMap<Key, Value>& scoped_map)
	: scoped_map_{ scoped_map }
	, level_{ scoped_map_.scope_begin() }
{}

template <typename Key, typename Value>
ScopedMap<Key, Value>::Scope::~Scope()
{
	scoped_map_.scope_end(level_);
}

template <typename Key, typename Value>
auto ScopedMap<Key, Value>::scope() -> Scope
{
	return Scope{ *this };
}

template <typename Key, typename Value>
void ScopedMap<Key, Value>::clear()
{
	if (scopes_.size() != 1) throw std::runtime_error{ "ScopedMap::clear: scope error" };
	scopes_.clear();
}

template <typename Key, typename Value>
void ScopedMap<Key, Value>::put(Key const& key, Value const& value)
{
	scopes_.back().insert_or_assign(key, value);
}

template <typename Key, typename Value>
std::optional<Value> ScopedMap<Key, Value>::get(Key const& key) const
{
	if (auto const it = scopes_.back().find(key); it != scopes_.back().end()) return it->second;
	return {};
}

template <typename Key, typename Value>
std::size_t ScopedMap<Key, Value>::scope_begin()
{
	scopes_.emplace_back();
	return scopes_.size();
}

template <typename Key, typename Value>
void ScopedMap<Key, Value>::scope_end(std::size_t level)
{
	if (scopes_.size() != level) throw std::runtime_error{ "ScopedMap::scope_end: scope error" };
	scopes_.pop_back();
}

} // namespace vat::utils
