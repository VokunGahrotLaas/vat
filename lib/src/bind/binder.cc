#include <vat/bind/binder.hh>

// STL
#include <iomanip>

namespace vat::bind
{

std::unordered_set<std::string> const Binder::static_vars_{
	"()", "unit", "int", "bool", "type", "!",
};

Binder::Binder(utils::ErrorManager& em)
	: error_{ em }
{}

void Binder::bind(ast::Ast& ast) { super_type::operator()(ast); }

void Binder::reset() { lets_.clear(); }

void Binder::operator()(ast::Name& name)
{
	auto opt = lets_.get(name.value());
	if (opt)
		name.let_exp(opt->get());
	else if (auto it = static_vars_.find(name.value()); it != static_vars_.end())
		name.let_exp(nullptr);
	else
	{
		error_.error(utils::ErrorType::Binding, name.location()) << "no such variable " << std::quoted(name.value());
		return;
	}
}

void Binder::operator()(ast::LetExp& let_exp)
{
	if (let_exp.has_type_name()) let_exp.type_name().accept(*this);
	if (!let_exp.is_rec() && let_exp.has_value()) let_exp.value().accept(*this);
	if (!lets_.insert(let_exp.name().value(), ast::shared_from_ast(let_exp)))
	{
		error_.error(utils::ErrorType::Binding, let_exp.location())
			<< "redefiniton of " << std::quoted(let_exp.name().value())
			<< "\nPrevious definition here: " << lets_[let_exp.name().value()]->location();
		return;
	}
	if (let_exp.is_rec() && let_exp.has_value()) let_exp.value().accept(*this);
	let_exp.name().accept(*this);
}

void Binder::operator()(ast::FnExp& fn_exp)
{
	auto scope = lets_.call_scope();
	super_type::operator()(fn_exp);
}

void Binder::operator()(ast::IfExp& if_exp)
{
	auto scope = lets_.scope();
	super_type::operator()(if_exp);
}

void Binder::operator()(ast::BlockExp& block_exp)
{
	auto scope = lets_.scope();
	super_type::operator()(block_exp);
}

} // namespace vat::bind
