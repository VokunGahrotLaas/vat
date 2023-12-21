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
class FnExp;
class CallExp;
class LetExp;

using SharedAst = std::shared_ptr<Ast>;
using SharedConstAst = std::shared_ptr<Ast const>;
using SharedExp = std::shared_ptr<Exp>;
using SharedConstExp = std::shared_ptr<Exp const>;
using SharedName = std::shared_ptr<Name>;
using SharedConstName = std::shared_ptr<Name const>;
using SharedAssignExp = std::shared_ptr<AssignExp>;
using SharedConstAssignExp = std::shared_ptr<AssignExp const>;
using SharedSeqExp = std::shared_ptr<SeqExp>;
using SharedConstSeqExp = std::shared_ptr<SeqExp const>;
using SharedFnExp = std::shared_ptr<FnExp>;
using SharedConstFnExp = std::shared_ptr<FnExp const>;
using SharedCallExp = std::shared_ptr<CallExp>;
using SharedConstCallExp = std::shared_ptr<CallExp const>;
using LetCallExp = std::shared_ptr<LetExp>;
using SharedConstLetExp = std::shared_ptr<LetExp const>;

template <template <typename> typename Const>
class GenVisitor;
using Visitor = GenVisitor<std::type_identity_t>;
using ConstVisitor = GenVisitor<std::add_const_t>;

} // namespace vat::ast
