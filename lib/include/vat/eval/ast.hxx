#pragma once

#include <vat/eval/ast.hh>

namespace vat::eval
{

inline auto AstEvaluator::variable(std::string const& name) -> std::optional<exp_type> { return variables_.get(name); }

} // namespace vat::eval
