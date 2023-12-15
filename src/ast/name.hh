#pragma once

// STL
#include <string_view>

// vat
#include "ast/exp.hh"

namespace vat::ast
{

class Name : public Exp
{
public:
	Name(Location const& location, std::string_view value);

	void accept(Visitor& visitor) override;
	void accept(ConstVisitor& visitor) const override;

	std::string_view value() const;

private:
	std::string value_;
};

} // namespace vat::ast

#include "ast/name.hxx"
