#pragma once

// STL
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

private:
	std::string value_;
};

} // namespace vat::ast

#include <vat/ast/name.hxx>
