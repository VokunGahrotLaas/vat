#include <vat/bind/binder.hh>

// STL
#include <stdexcept>

namespace vat::bind
{

bool Binder::bind(ast::Ast& ast)
{
	try
	{
		super_type::operator()(ast);
	}
	catch (std::exception const& e)
	{
		std::cerr << "Binder::" << e.what() << std::endl;
		return false;
	}
	return true;
}

void Binder::reset() { lets_.clear(); }

void Binder::operator()(ast::Name& name)
{
	auto opt = lets_.get(name.value());
	if (!opt) throw std::runtime_error{ "Name: no such variable " + name.value() };
	name.let_exp(opt->get());
}

void Binder::operator()(ast::LetExp& let_exp)
{
	lets_.insert_or_assign(let_exp.name().value(), ast::shared_from_ast(let_exp));
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
