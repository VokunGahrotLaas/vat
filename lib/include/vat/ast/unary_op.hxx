#pragma once

#include <vat/ast/unary_op.hh>

namespace vat::ast
{

inline auto UnaryOp::oper() const -> Oper { return oper_; }

inline Exp const& UnaryOp::value() const { return *value_; }

inline Exp& UnaryOp::value() { return *value_; }

constexpr std::string_view UnaryOp::str(Oper oper)
{
	switch (oper)
	{
	case Pos: return "+";
	case Neg: return "-";
	default: return "[[UnaryOp: unnamed]]";
	}
}

} // namespace vat::ast
