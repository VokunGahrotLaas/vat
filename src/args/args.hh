#pragma once

// STL
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vat
{

class ParsedArgs {
public:
private:
};

class Args
{
public:
	Args() = default;

	ParsedArgs parse(std::span<std::string_view const> args) const;

private:
};

} // namespace vat
