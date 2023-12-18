#pragma once

// vat
#include <memory>
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

private:
	Location location_;
};

} // namespace vat::ast

#include <vat/ast/ast.hxx>
