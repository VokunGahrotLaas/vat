#pragma once

#include "ast/compute_visitor.hh"

namespace vat::ast
{

inline std::optional<int> ComputeVisitor::result() const { return result_; }

inline std::optional<int> ComputeVisitor::variable(std::string const& name)
{
	if (auto const it = variables_.find(name); it != variables_.end()) return it->second;
	return {};
}

inline void ComputeVisitor::reset_variables() { variables_.clear(); }

} // namespace vat::ast
