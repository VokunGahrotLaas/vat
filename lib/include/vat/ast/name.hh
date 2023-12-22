#pragma once

// STL
#include <optional>
#include <string_view>

// vat
#include <vat/ast/exp.hh>

namespace vat::ast
{

class Name : public Exp
{
public:
	Name(Location const& location, std::string_view value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	std::string const& value() const;
	SharedConstLetExp let_exp() const;
	void let_exp(SharedConstLetExp let_exp);

private:
	std::string value_;
	SharedConstLetExp let_exp_;
};

} // namespace vat::ast

#include <vat/ast/name.hxx>
