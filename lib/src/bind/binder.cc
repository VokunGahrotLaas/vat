#include <vat/bind/binder.hh>

// STL
#include <iomanip>

namespace vat::bind
{

Binder::Binder(utils::ErrorManager& em)
	: error_{ em }
{}

void Binder::bind(ast::Ast& ast) { super_type::operator()(ast); }

void Binder::reset() { lets_.clear(); }

void Binder::operator()(ast::Name& name)
{
	auto opt = lets_.get(name.value());
	if (!opt)
	{
		error_.error(utils::ErrorType::Binding, name.location()) << "no such variable " << std::quoted(name.value());
		return;
	}
	name.let_exp(opt->get());
}

void Binder::operator()(ast::LetExp& let_exp)
{
	if (!lets_.insert(let_exp.name().value(), ast::shared_from_ast(let_exp)))
	{
		error_.error(utils::ErrorType::Binding, let_exp.location())
			<< "redefiniton of " << std::quoted(let_exp.name().value())
			<< "\nPrevious definition here: " << lets_[let_exp.name().value()]->location();
		return;
	}
	super_type::operator()(let_exp);
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

} // namespace vat::bind
