#pragma once

#include <vat/type/type_checker.hh>

namespace vat::type
{

inline void TypeChecker::type(ast::Ast& ast) { super_type::operator()(ast); }

inline void TypeChecker::reset()
{
	recurse_body_ = true;
	constexpr_eval_.reset();
}

inline bool TypeChecker::check_assignable(ast::Location const& location, Type const& to, Type const& from)
{
	if (to.assignable_from(from)) return true;
	error_.error(utils::ErrorType::Typing, location)
		<< "incompatible type:\n    expected: " << to << "\n    got: " << from;
	return false;
}

inline bool TypeChecker::check_same(ast::Location const& location, Type const& expected, Type const& got)
{
	if (expected == got) return true;
	error_.error(utils::ErrorType::Typing, location)
		<< "incompatible type:\n    expected: " << expected << "\n    got: " << got;
	return false;
}

} // namespace vat::type
