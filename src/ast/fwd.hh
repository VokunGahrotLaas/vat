#pragma once

// STL
#include <type_traits>

namespace vat::ast
{

class Ast;
class Exp;
class Number;
class Name;
class UnaryOp;
class BinaryOp;

template <template <typename> typename Const>
class GenVisitor;
using Visitor = GenVisitor<std::type_identity_t>;
using ConstVisitor = GenVisitor<std::add_const_t>;

} // namespace vat::ast
