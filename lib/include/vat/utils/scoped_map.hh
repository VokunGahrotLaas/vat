#pragma once

// STL
#include <optional>
#include <unordered_map>
#include <vector>

namespace vat::utils
{

template <typename Key, typename Value>
class ScopedMap final
{
public:
	using map = std::unordered_map<Key, Value>;
	using vec = std::vector<map>;

	class Scope final
	{
	public:
		Scope(ScopedMap<Key, Value>& scoped_map);
		~Scope();

	private:
		ScopedMap<Key, Value>& scoped_map_;
		std::size_t level_;
	};

	Scope scope();
	void clear();
	void put(Key const& key, Value const& value);
	std::optional<Value> get(Key const& key) const;

private:
	std::size_t scope_begin();
	void scope_end(std::size_t level);

	vec scopes_{ map{} };
};

} // namespace vat::utils

#include <vat/utils/scoped_map.hxx>
