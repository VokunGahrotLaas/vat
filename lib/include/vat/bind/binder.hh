#pragma once

// vat
#include <vat/ast/default_visitor.hh>
#include <vat/utils/scoped_map.hh>

namespace vat::bind
{

class Binder : private ast::DefaultVisitor
{
public:
	using super_type = ast::DefaultVisitor;

	~Binder() override = default;

	bool bind(ast::Ast& ast);
	void reset();

private:
	// lets
	void operator()(ast::Name& name) override;
	void operator()(ast::LetExp& let_exp) override;

	// scope
	void operator()(ast::FnExp& fn_exp) override;
	void operator()(ast::IfExp& if_exp) override;

	utils::ScopedMap<std::string_view, ast::SharedConstLetExp> lets_{};
};

} // namespace vat::bind
