#pragma once

#include "ast/location.hh"
#include "ast/visitor.hh"

namespace vat::ast
{

class Ast
{
public:
	explicit Ast(Location const& location);
	Ast(Ast const&) = delete;
	Ast& operator=(Ast const&) = delete;
	virtual ~Ast() = default;

	virtual void accept(Visitor&) = 0;
	virtual void accept(ConstVisitor&) const = 0;

	Location const& location() const;

private:
	Location location_;
};

} // namespace vat::ast
