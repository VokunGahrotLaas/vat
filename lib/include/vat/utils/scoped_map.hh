#pragma once

// STL
#include <initializer_list>
#include <optional>
#include <unordered_map>
#include <vector>

namespace vat::utils
{

class ScopedMapDebug
{
public:
	static void set(bool debug);
	static bool get();

private:
	static bool debug_;
};

template <typename Key, typename Value>
class ScopedMap final
{
public:
	class Scope final
	{
	public:
		friend class ScopedMap;

		~Scope();

	private:
		Scope(ScopedMap& scoped_map, std::size_t level);

		ScopedMap<Key, Value>& scoped_map_;
		std::size_t level_;
	};

	~ScopedMap();

	Scope scope();
	Scope call_scope();
	void clear();
	void insert_or_assign(Key const& key, Value const& value);
	bool insert(Key const& key, Value const& value);
	std::optional<std::reference_wrapper<Value const>> get(Key const& key) const;
	std::optional<std::reference_wrapper<Value>> get(Key const& key);
	Value const& operator[](Key const& key) const;
	Value& operator[](Key const& key);
	std::size_t level() const;

private:
	void scope_end(std::size_t level);

	using map_type = std::unordered_map<Key, Value>;

	struct Level
	{
		map_type map{};
		std::size_t prev{};
		std::size_t overriden{};
	};

	using vec_type = std::vector<Level>;

	vec_type scopes_{ {} };
};

} // namespace vat::utils

#include <vat/utils/scoped_map.hxx>
