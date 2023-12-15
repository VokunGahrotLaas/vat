#pragma once

// STL
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

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

	std::shared_ptr<ast::Ast> get_result();
	void set_result(std::shared_ptr<ast::Ast> result);

	std::shared_ptr<ast::Exp> get_variable(std::string const& name) const;
	void set_variable(std::string const& name, std::shared_ptr<ast::Exp> value);

private:
	std::string filename_;
	bool trace_parsing_;
	bool trace_scanning_;
	std::unordered_map<std::string, std::shared_ptr<ast::Exp>> variables_{};
	parser::location loc_;
	std::istream* is_{ nullptr };
	std::ostream* os_{ nullptr };
	std::unique_ptr<Lexer> lexer_;
	bool finished_{ false };
	bool success_{ false };
	std::shared_ptr<ast::Ast> result_{};
};

} // namespace vat::parser

#include "parser/driver.hxx"
