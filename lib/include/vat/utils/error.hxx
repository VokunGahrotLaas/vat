#pragma once

#include <vat/utils/error.hh>

namespace vat::utils
{

template <typename T>
auto ErrorManager::Error::operator<<(T&& arg) -> Error&
{
	oss_ << std::forward<T>(arg);
	return *this;
}

inline auto ErrorManager::Error::operator<<(func_type func) -> Error&
{
	oss_ << func;
	return *this;
}

inline ErrorManager::Error::Error(Type type, parser::location const& location)
	: oss_{}
	, location_{ location }
	, type_{ type }
{}

inline auto ErrorManager::error(Type type, parser::location const& location) -> Error&
{
	if (type == None) throw std::runtime_error{ "ErrorManager: cannot create an error with None (use Other)" };
	type_ |= type;
	errors_.push_back(Error{ type, location });
	return errors_.back();
}

inline std::ostream& operator<<(std::ostream& os, ErrorManager::Error const& err)
{
	return os << err.location_ << ": " << enum_str(err.type_) << ":\n" << err.oss_.str() << '\n';
}

inline std::ostream& operator<<(std::ostream& os, ErrorManager const& em)
{
	for (ErrorManager::Error const& err: em.errors_)
		os << err;
	return os;
}

inline auto ErrorManager::type() const -> Type { return type_; }

inline ErrorManager::operator bool() const { return type() != None; }

inline std::string_view enum_str(ErrorType type)
{
	switch (type)
	{
	case ErrorType::None: return "not an error";
	case ErrorType::Other: return "other error";
	case ErrorType::Lexing: return "lexing error";
	case ErrorType::Parsing: return "parsing error";
	case ErrorType::Binding: return "binding error";
	case ErrorType::Typing: return "typing error";
	case ErrorType::Evaluation: return "evaluation error";
	default: return "unnamed error";
	}
}

inline int enum_code(ErrorType type)
{
	if (type & ErrorType::Other) return 1;
	if (type & ErrorType::Lexing) return 2;
	if (type & ErrorType::Parsing) return 3;
	if (type & ErrorType::Binding) return 4;
	if (type & ErrorType::Typing) return 5;
	if ((type & ErrorType::Evaluation) != ErrorType::None) return 6;
	return 0;
}

inline ErrorType operator|(ErrorType lhs, ErrorType rhs)
{
	return static_cast<ErrorType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline ErrorType& operator|=(ErrorType& lhs, ErrorType rhs) { return (lhs = lhs | rhs); }

inline ErrorType operator&(ErrorType lhs, ErrorType rhs)
{
	return static_cast<ErrorType>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

inline ErrorType& operator&=(ErrorType& lhs, ErrorType rhs) { return (lhs = lhs & rhs); }

} // namespace vat::utils
