#pragma once

// STL
#include <optional>
#include <string_view>
#include <unordered_map>

// vat
#include <vat/parse/driver.hh>
#include <vat/utils/error.hh>

namespace vat::parse
{

class Parser
{
public:
	Parser(utils::ErrorManager& em);

	ast::SharedAst parse(std::string_view filename);

	void set_trace_parsing(bool value);
	void set_trace_scanning(bool value);

private:
	std::unordered_map<std::string_view, std::unique_ptr<Driver>> drivers_{};
	bool trace_parsing_{ false };
	bool trace_scanning_{ false };
	utils::ErrorManager& error_;
};

} // namespace vat::parse
