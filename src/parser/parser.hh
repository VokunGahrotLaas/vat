#pragma once

// STL
#include <optional>
#include <string_view>
#include <unordered_map>

// vat
#include "parser/driver.hh"

namespace vat::parser
{
class Parser
{
public:
	Parser() = default;

	std::shared_ptr<ast::Ast> parse(std::string_view filename);

	void set_trace_parsing(bool value);
	void set_trace_scanning(bool value);

private:
	std::unordered_map<std::string, std::unique_ptr<Driver>> drivers_{};
	bool trace_parsing_{ false };
	bool trace_scanning_{ false };
};

} // namespace vat::parser
