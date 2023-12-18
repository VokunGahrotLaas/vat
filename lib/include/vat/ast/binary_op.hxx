#pragma once

#include <vat/ast/binary_op.hh>

namespace vat::ast
{

inline auto BinaryOp::oper() const -> Oper { return oper_; }

inline Exp const& BinaryOp::lhs() const { return *lhs_; }

inline Exp const& BinaryOp::rhs() const { return *rhs_; }

inline Exp& BinaryOp::lhs() { return *lhs_; }

inline Exp& BinaryOp::rhs() { return *rhs_; }

constexpr std::string_view BinaryOp::str(Oper oper)
{
	switch (oper)
	{
	case Add: return "+";
	case Sub: return "-";
	case Mul: return "*";
	case Div: return "/";
	case Mod: return "%";
	case Pow: return "**";
	default: return "[[BinaryOp: unnamed]]";
	}
}

} // namespace vat::ast
