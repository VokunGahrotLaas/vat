#pragma once

// STL
#include <memory>
#include <type_traits>

namespace vat::ast
{

class Ast;
class Exp;
class AssignExp;
class SeqExp;
class Number;
class Name;
class UnaryOp;
class BinaryOp;

using SharedAst = std::shared_ptr<Ast>;
using SharedConstAst = std::shared_ptr<Ast const>;
using SharedExp = std::shared_ptr<Exp>;
using SharedConstExp = std::shared_ptr<Exp const>;
using SharedName = std::shared_ptr<Name>;
using SharedConstName = std::shared_ptr<Name const>;
using SharedAssignExp = std::shared_ptr<AssignExp>;
using SharedConstAssignExp = std::shared_ptr<AssignExp const>;

template <template <typename> typename Const>
class GenVisitor;
using Visitor = GenVisitor<std::type_identity_t>;
using ConstVisitor = GenVisitor<std::add_const_t>;

} // namespace vat::ast
