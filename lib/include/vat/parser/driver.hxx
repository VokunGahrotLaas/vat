#pragma once

#include <vat/parser/driver.hh>

namespace vat::parser
{

inline std::ostream& Driver::yyout() { return *os_; }

inline Lexer& Driver::lexer() { return *lexer_; }

inline parser::location& Driver::location() { return loc_; }

inline utils::ErrorManager::Error& Driver::error(utils::ErrorType type) { return error(type, location()); }

inline utils::ErrorManager::Error& Driver::error(utils::ErrorType type, parser::location const& loc)
{
	return error_.error(type, loc);
}

inline ast::SharedAst Driver::get_result() { return result_; }

inline void Driver::set_result(ast::SharedAst result) { result_ = std::move(result); }

inline std::string_view Driver::filename() const { return filename_; }

} // namespace vat::parser
