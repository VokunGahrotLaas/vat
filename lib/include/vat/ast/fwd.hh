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
class Bool;
class IfExp;
class Unit;

using SharedAst = std::shared_ptr<Ast>;
using SharedConstAst = std::shared_ptr<Ast const>;
using SharedExp = std::shared_ptr<Exp>;
using SharedConstExp = std::shared_ptr<Exp const>;
using SharedAssignExp = std::shared_ptr<AssignExp>;
using SharedConstAssignExp = std::shared_ptr<AssignExp const>;
using SharedSeqExp = std::shared_ptr<SeqExp>;
using SharedConstSeqExp = std::shared_ptr<SeqExp const>;
using SharedName = std::shared_ptr<Name>;
using SharedConstName = std::shared_ptr<Name const>;
using SharedUnaryOp = std::shared_ptr<UnaryOp>;
using SharedConstUnaryOp = std::shared_ptr<UnaryOp const>;
using SharedBinaryOp = std::shared_ptr<BinaryOp>;
using SharedConstBinaryOp = std::shared_ptr<BinaryOp const>;
using SharedFnExp = std::shared_ptr<FnExp>;
using SharedConstFnExp = std::shared_ptr<FnExp const>;
using SharedCallExp = std::shared_ptr<CallExp>;
using SharedConstCallExp = std::shared_ptr<CallExp const>;
using SharedLetExp = std::shared_ptr<LetExp>;
using SharedConstLetExp = std::shared_ptr<LetExp const>;
using SharedBool = std::shared_ptr<Bool>;
using SharedConstBool = std::shared_ptr<Bool const>;
using SharedIfExp = std::shared_ptr<IfExp>;
using SharedConstIfExp = std::shared_ptr<IfExp const>;
using SharedUnit = std::shared_ptr<Unit>;
using SharedConstUnit = std::shared_ptr<Unit const>;

template <template <typename> typename Const>
class GenVisitor;
using Visitor = GenVisitor<std::type_identity_t>;
using ConstVisitor = GenVisitor<std::add_const_t>;

} // namespace vat::ast
