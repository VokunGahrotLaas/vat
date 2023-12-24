#pragma once

// STL
#include <memory>
#include <optional>
#include <string>

// vat
#include <vat/ast/fwd.hh>
#include <vat/parser/location.hh>
#include <vat/utils/error.hh>

namespace vat::parser
{

class Lexer;

class Driver
{
public:
	Driver(std::string_view filename, utils::ErrorManager& em, bool trace_parsing, bool trace_scanning);
	~Driver();

	bool parse();

	std::ostream& yyout();
	Lexer& lexer();
	parser::location& location();
	utils::ErrorManager::Error& error(utils::ErrorType type);
	utils::ErrorManager::Error& error(utils::ErrorType type, parser::location const& loc);

	ast::SharedAst get_result();
	void set_result(ast::SharedAst result);

	std::string_view filename() const;

private:
	std::string filename_;
	utils::ErrorManager& error_;
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

#include <vat/parser/driver.hxx>
