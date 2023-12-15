#pragma once

#include "parser/driver.hh"

namespace vat::parser
{

inline std::ostream& Driver::yyout() { return *os_; }

inline Lexer& Driver::lexer() { return *lexer_; }

inline parser::location& Driver::location() { return loc_; }

inline std::shared_ptr<ast::Ast> Driver::get_result() { return result_; }

inline void Driver::set_result(std::shared_ptr<ast::Ast> result) { result_ = std::move(result); }

inline std::shared_ptr<ast::Exp> Driver::get_variable(std::string const& name) const
{
	if (auto const it = variables_.find(name); it != variables_.end()) return it->second;
	return {};
}

inline void Driver::set_variable(std::string const& name, std::shared_ptr<ast::Exp> value)
{
	variables_[name] = std::move(value);
}

} // namespace vat::parser
