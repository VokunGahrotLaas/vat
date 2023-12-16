#pragma once

#include <vat/parser/driver.hh>

namespace vat::parser
{

inline std::ostream& Driver::yyout() { return *os_; }

inline Lexer& Driver::lexer() { return *lexer_; }

inline parser::location& Driver::location() { return loc_; }

inline ast::SharedAst Driver::get_result() { return result_; }

inline void Driver::set_result(ast::SharedAst result) { result_ = std::move(result); }

} // namespace vat::parser
