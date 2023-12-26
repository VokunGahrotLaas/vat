#include <vat/type/type_checker.hh>

// vat
#include <vat/eval/ast.hh>
#include <vat/type/all.hh>
#include <vat/utils/utils.hh>

namespace vat::type
{

std::unordered_map<std::string, SharedConstType> const TypeChecker::static_vars_{
	{"int",	 TypeType::instance()},
	  { "bool", TypeType::instance()},
	{ "unit", TypeType::instance()},
	{ "type", TypeType::instance()},
	  { "!",	 TypeType::instance()},
	{ "()",	Unit::instance()	 },
};

TypeChecker::TypeChecker(utils::ErrorManager& em)
	: error_{ em }
{}

void TypeChecker::operator()(ast::AssignExp& assign_exp)
{
	super_type::operator()(assign_exp);
	check_assignable(assign_exp.location(), *assign_exp.name().type(), *assign_exp.value().type());
	assign_exp.type(assign_exp.name().type());
}

void TypeChecker::operator()(ast::SeqExp& seq_exp)
{
	super_type::operator()(seq_exp);
	if (seq_exp.size() > 0) seq_exp.type(seq_exp.back().type());
}

void TypeChecker::operator()(ast::Number& number) { number.type(Number::instance()); }

void TypeChecker::operator()(ast::Name& name)
{
	if (name.let_exp() != nullptr)
		name.type(std::make_shared<Name const>(name.value(), name.let_exp()->type()));
	else if (auto it = static_vars_.find(name.value()); it != static_vars_.end())
		name.type(std::make_shared<Name const>(name.value(), it->second));
	else
		utils::unreachable();
	if (static_cast<Name const&>(*name.type()).is_chain())
	{
		error_.error(utils::ErrorType::Typing, name.location()) << "self recursive name";
		name.type(Never::instance());
	}
}

void TypeChecker::operator()(ast::UnaryOp& unary_op)
{
	super_type::operator()(unary_op);
	check_same(unary_op.location(), *Number::instance(), *unary_op.value().type());
	unary_op.type(Number::instance());
}

void TypeChecker::operator()(ast::BinaryOp& binary_op)
{
	super_type::operator()(binary_op);
	check_same(binary_op.location(), *Number::instance(), *binary_op.lhs().type());
	check_same(binary_op.location(), *Number::instance(), *binary_op.rhs().type());
	switch (binary_op.oper())
	{
	case ast::BinaryOp::Add: [[fallthrough]];
	case ast::BinaryOp::Sub: [[fallthrough]];
	case ast::BinaryOp::Mul: [[fallthrough]];
	case ast::BinaryOp::Div: [[fallthrough]];
	case ast::BinaryOp::Mod: [[fallthrough]];
	case ast::BinaryOp::Pow: binary_op.type(Number::instance()); break;
	case ast::BinaryOp::Eq: [[fallthrough]];
	case ast::BinaryOp::Ne: [[fallthrough]];
	case ast::BinaryOp::Lt: [[fallthrough]];
	case ast::BinaryOp::Le: [[fallthrough]];
	case ast::BinaryOp::Gt: [[fallthrough]];
	case ast::BinaryOp::Ge: binary_op.type(Bool::instance()); break;
	default: utils::unreachable();
	}
}

void TypeChecker::operator()(ast::FnExp& fn_exp)
{
	fn_exp.args().accept(*this);
	fn_exp.return_type().accept(*this);
	SharedConstType return_type = Never::instance();
	check_same(fn_exp.return_type().location(), *TypeType::instance(), *fn_exp.return_type().type());
	auto res = constexpr_eval_.eval(fn_exp.return_type());
	auto ptr = std::get_if<SharedConstType>(&res);
	if (!ptr)
		error_.error(utils::ErrorType::Typing, fn_exp.return_type().location())
			<< "fn's return type did not return a type";
	else
		return_type = *ptr;
	std::vector<SharedConstType> args(fn_exp.args().size());
	for (std::size_t i = 0; i < args.size(); ++i)
		args[i] = fn_exp.args()[i].type();
	fn_exp.type(std::make_shared<Fn const>(return_type, std::move(args)));
	if (recurse_body_)
	{
		fn_exp.body().accept(*this);
		check_assignable(fn_exp.return_type().location(), *return_type, *fn_exp.body().type());
	}
}

void TypeChecker::operator()(ast::CallExp& call_exp)
{
	super_type::operator()(call_exp);
	if (call_exp.function().type() == nullptr)
	{
		error_.error(utils::ErrorType::Typing, call_exp.function().location()) << "calling nullptr";
		call_exp.type(Never::instance());
		return;
	}
	auto fn = dynamic_cast<Fn const*>(&call_exp.function().type()->resolve());
	if (!fn)
	{
		error_.error(utils::ErrorType::Typing, call_exp.function().location())
			<< "calling non function type: " << *call_exp.function().type();
		call_exp.type(Never::instance());
		return;
	}
	std::vector<SharedConstType> args(call_exp.args().size());
	for (std::size_t i = 0; i < args.size(); ++i)
		args[i] = call_exp.args()[i].type();
	auto call_type = std::make_shared<Fn const>(fn->return_type(), std::move(args));
	check_assignable(call_exp.location(), *fn, *call_type);
	call_exp.type(fn->return_type());
}

void TypeChecker::operator()(ast::LetExp& let_exp)
{
	SharedConstType type_name{};
	if (let_exp.has_type_name())
	{
		let_exp.type_name().accept(*this);
		check_same(let_exp.type_name().location(), *TypeType::instance(), *let_exp.type_name().type());
		auto res = constexpr_eval_.eval(let_exp.type_name());
		auto ptr = std::get_if<SharedConstType>(&res);
		if (!ptr)
		{
			error_.error(utils::ErrorType::Typing, let_exp.type_name().location())
				<< "let's type name did not return a type";
			let_exp.type(Never::instance());
			return;
		}
		type_name = *ptr;
		let_exp.type(type_name);
		let_exp.name().accept(*this);
	}
	if (let_exp.has_value())
	{
		bool old_recurse_body = recurse_body_;
		recurse_body_ = false;
		let_exp.value().accept(*this);
		recurse_body_ = old_recurse_body;
	}
	if (!let_exp.has_type_name())
	{
		let_exp.type(let_exp.value().type());
		let_exp.name().accept(*this);
	}
	if (let_exp.has_value())
	{
		bool old_recurse_body = recurse_body_;
		recurse_body_ = true;
		let_exp.value().accept(*this);
		recurse_body_ = old_recurse_body;
	}
	if (let_exp.has_value() && let_exp.has_type_name())
		check_assignable(let_exp.location(), *type_name, *let_exp.value().type());
	if (let_exp.has_value()) constexpr_eval_.eval(let_exp);
}

void TypeChecker::operator()(ast::Bool& bool_exp) { bool_exp.type(Bool::instance()); }

void TypeChecker::operator()(ast::IfExp& if_exp)
{
	super_type::operator()(if_exp);
	check_same(if_exp.cond().location(), *Bool::instance(), *if_exp.cond().type());
	check_same(if_exp.location(), *if_exp.then_exp().type(), *if_exp.else_exp().type());
	if_exp.type(if_exp.then_exp().type());
}

void TypeChecker::operator()(ast::BlockExp& block_exp)
{
	super_type::operator()(block_exp);
	block_exp.type(block_exp.exp().type());
}

} // namespace vat::type
