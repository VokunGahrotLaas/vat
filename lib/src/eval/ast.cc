#include <vat/eval/ast.hh>

// STL
#include <cmath>
#include <stdexcept>

// vat
#include <vat/utils/utils.hh>
#include <vat/utils/variant.hh>

namespace vat::eval
{

std::unordered_map<std::string, AstEvaluator::exp_type> const AstEvaluator::static_vars_{
	{"int",	 type::Number::instance()  },
	{ "bool", type::Bool::instance()	 },
	{ "unit", type::Unit::instance()	 },
	{ "type", type::TypeType::instance()},
	{ "!",	   type::Never::instance()   },
	{ "()",	{}						   },
};

AstEvaluator::AstEvaluator(utils::ErrorManager& em)
	: error_{ em }
{}

void AstEvaluator::operator()(ast::Ast const& ast) { ast.accept(*this); }

void AstEvaluator::operator()(ast::Exp const& exp) { exp.accept(*this); }

void AstEvaluator::operator()(ast::AssignExp const& assign_exp)
{
	exp_type& old_value = vars_[assign_exp.name().let_exp()];
	assign_exp.value().accept(*this);
	old_value = result_;
}

void AstEvaluator::operator()(ast::SeqExp const& seq_exp)
{
	for (ast::SharedConstExp const& exp: seq_exp.exps())
		exp->accept(*this);
}

void AstEvaluator::operator()(ast::Number const& number) { result_ = number.value(); }

void AstEvaluator::operator()(ast::Name const& name)
{
	if (name.let_exp() != nullptr)
		result_ = vars_[name.let_exp()];
	else if (auto it = static_vars_.find(name.value()); it != static_vars_.end())
		result_ = it->second;
	else
		utils::unreachable();
}

void AstEvaluator::operator()(ast::UnaryOp const& unary_op)
{
	unary_op.value().accept(*this);
	int& result = std::get<int>(result_);
	switch (unary_op.oper())
	{
	case ast::UnaryOp::Pos: break;
	case ast::UnaryOp::Neg: result = -result; break;
	default: utils::unreachable();
	}
}

void AstEvaluator::operator()(ast::BinaryOp const& binary_op)
{
	binary_op.lhs().accept(*this);
	int lhs = std::get<int>(result_);
	binary_op.rhs().accept(*this);
	int rhs = std::get<int>(result_);
	switch (binary_op.oper())
	{
	case ast::BinaryOp::Add: result_ = lhs + rhs; break;
	case ast::BinaryOp::Sub: result_ = lhs - rhs; break;
	case ast::BinaryOp::Mul: result_ = lhs * rhs; break;
	case ast::BinaryOp::Div:
		if (rhs == 0)
		{
			error_.error(utils::ErrorType::Evaluation, binary_op.location()) << "division by zero";
			result_ = 0;
			break;
		}
		result_ = lhs / rhs;
		break;
	case ast::BinaryOp::Mod: result_ = lhs % rhs; break;
	case ast::BinaryOp::Pow: result_ = static_cast<int>(std::pow(lhs, rhs)); break;
	case ast::BinaryOp::Eq: result_ = lhs == rhs; break;
	case ast::BinaryOp::Ne: result_ = lhs != rhs; break;
	case ast::BinaryOp::Lt: result_ = lhs < rhs; break;
	case ast::BinaryOp::Le: result_ = lhs <= rhs; break;
	case ast::BinaryOp::Gt: result_ = lhs > rhs; break;
	case ast::BinaryOp::Ge: result_ = lhs >= rhs; break;
	default: utils::unreachable();
	}
}

void AstEvaluator::operator()(ast::FnExp const& fn_exp) { result_ = shared_from_ast(fn_exp); }

void AstEvaluator::operator()(ast::CallExp const& call_exp)
{
	call_exp.function().accept(*this);
	ast::FnExp const& fn_exp = *std::get<ast::SharedConstFnExp>(result_);
	std::vector<exp_type> args(call_exp.args().size());
	for (std::size_t i = 0; i < args.size(); ++i)
	{
		call_exp.args()[i].accept(*this);
		args[i] = result_;
	}
	auto call_scope = vars_.call_scope();
	for (std::size_t i = 0; i < args.size(); ++i)
		vars_.insert_or_assign(ast::shared_from_ast(static_cast<ast::LetExp const&>(fn_exp.args()[i])), args[i]);
	fn_exp.body().accept(*this);
}

void AstEvaluator::operator()(ast::LetExp const& let_exp)
{
	let_exp.value().accept(*this);
	vars_.insert_or_assign(ast::shared_from_ast(let_exp), result_);
}

void AstEvaluator::operator()(ast::Bool const& bool_exp) { result_ = bool_exp.value(); }

void AstEvaluator::operator()(ast::IfExp const& if_exp)
{
	if_exp.cond().accept(*this);
	(std::get<bool>(result_) ? if_exp.then_exp() : if_exp.else_exp()).accept(*this);
}

void AstEvaluator::operator()(ast::BlockExp const& block_exp) { block_exp.exp().accept(*this); }

auto AstEvaluator::eval(input_type input) -> exp_type
{
	operator()(input);
	return result_;
}

void AstEvaluator::reset()
{
	result_ = {};
	vars_.clear();
}

void AstEvaluator::print_exp(std::ostream& os, exp_type exp)
{
	std::visit(utils::overloads{ [&os](type::SharedConstType type) { os << "(type) " << *type; },
								 [&os](int v) { os << "(int) " << v; }, [&os](std::monostate) { os << "(unit) ()"; },
								 [&os](bool v) { os << "(bool) " << (v ? "true" : "false"); },
								 [&os](ast::SharedConstFnExp v) { os << "(fn) " << v->location(); } },
			   exp);
}

} // namespace vat::eval
