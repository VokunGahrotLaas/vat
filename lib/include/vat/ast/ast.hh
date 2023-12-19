#pragma once

// STL
#include <concepts>
#include <memory>

// vat
#include <vat/ast/location.hh>
#include <vat/ast/visitor.hh>

namespace vat::ast
{

class Ast : public std::enable_shared_from_this<Ast>
{
public:
	explicit Ast(Location const& location);
	Ast(Ast const&) = delete;
	Ast(Ast&&) = delete;
	Ast& operator=(Ast const&) = delete;
	Ast& operator=(Ast&&) = delete;
	virtual ~Ast() = default;

	virtual void accept(Visitor&) = 0;
	virtual void accept(ConstVisitor&) const = 0;

	Location const& location() const;
	void location(Location const& location);

	template <std::derived_from<Ast> T>
	friend std::shared_ptr<T> shared_from_ast(T& ast);
	template <std::derived_from<Ast> T>
	friend std::shared_ptr<T const> shared_from_ast(T const& ast);

private:
	Location location_;
};

} // namespace vat::ast

#include <vat/ast/ast.hxx>
