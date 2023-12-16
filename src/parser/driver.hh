#pragma once

// STL
#include <memory>
#include <optional>
#include <string>

// vat
#include "ast/fwd.hh"
#include "parser/location.hh"

namespace vat::parser
{

class Lexer;

class Driver
{
public:
	Driver(std::string_view filename, bool trace_parsing, bool trace_scanning);
	~Driver();

	bool parse();

	std::ostream& yyout();
	Lexer& lexer();
	parser::location& location();

	ast::SharedAst get_result();
	void set_result(ast::SharedAst result);

private:
	std::string filename_;
	bool trace_parsing_;
	bool trace_scanning_;
	parser::location loc_;
	std::istream* is_{ nullptr };
	std::ostream* os_{ nullptr };
	std::unique_ptr<Lexer> lexer_;
	bool finished_{ false };
	bool success_{ false };
	ast::SharedAst result_{};
};

} // namespace vat::parser

#include "parser/driver.hxx"
