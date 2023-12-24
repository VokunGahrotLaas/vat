#pragma once

// STL
#include <exception>
#include <sstream>
#include <vector>

// vat
#include <vat/parser/location.hh>

namespace vat::utils
{

class ErrorManager
{
public:
	enum Type
	{
		None = 0,
		Other = 1,
		Lexing = 2,
		Parsing = 4,
		Binding = 8,
		Typing = 16,
		Evaluation = 32,
	};

	struct Error
	{
	public:
		friend ErrorManager;

		using func_type = std::ostream& (*)(std::ostream&);

		template <typename T>
		Error& operator<<(T&& arg);

		Error& operator<<(func_type func);

		friend std::ostream& operator<<(std::ostream& os, Error const& err);

	private:
		Error(Type type, parser::location const& location);

		std::ostringstream oss_;
		parser::location location_;
		Type type_;
	};

	Error& error(Type type, parser::location const& location);

	friend std::ostream& operator<<(std::ostream& os, ErrorManager const& em);

	Type type() const;
	operator bool() const;

private:
	std::vector<Error> errors_{};
	Type type_{ None };
};

using ErrorType = ErrorManager::Type;

std::string_view enum_str(ErrorType type);
int enum_code(ErrorType type);
ErrorType operator|(ErrorType lhs, ErrorType rhs);
ErrorType& operator|=(ErrorType& lhs, ErrorType rhs);
ErrorType operator&(ErrorType lhs, ErrorType rhs);
ErrorType& operator&=(ErrorType& lhs, ErrorType rhs);

} // namespace vat::utils

#include <vat/utils/error.hxx>
