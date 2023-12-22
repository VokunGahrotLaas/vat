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
		friend class ScopedMap;

		~Scope();

	private:
		Scope(ScopedMap& scoped_map, bool call = false);

		ScopedMap<Key, Value>& scoped_map_;
		std::size_t level_;
	};

	Scope scope();
	Scope call_scope();
	void clear();
	void insert_or_assign(Key const& key, Value const& value);
	bool insert(Key const& key, Value const& value);
	std::optional<Value> get(Key const& key) const;
	std::optional<std::reference_wrapper<Value>> get(Key const& key);

private:
	std::size_t scope_begin();
	std::size_t call_scope_begin();
	void scope_end(std::size_t level);

	vec scopes_{ map{} };
};

} // namespace vat::utils

#include <vat/utils/scoped_map.hxx>
