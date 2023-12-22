#include <vat/parser/parser.hh>

namespace vat::parser
{

ast::SharedAst Parser::parse(std::string_view filename)
{
	if (auto const it = drivers_.find(filename); it != drivers_.end())
	{
		if (!it->second->parse()) return {};
		return it->second->get_result();
	}

	auto driver = std::make_unique<Driver>(filename, trace_parsing_, trace_scanning_);
	auto [it, _] = drivers_.insert({ driver->filename(), std::move(driver) });
	if (!it->second->parse()) return {};
	return it->second->get_result();
}

void Parser::set_trace_parsing(bool value) { trace_parsing_ = value; }

void Parser::set_trace_scanning(bool value) { trace_scanning_ = value; }

} // namespace vat::parser
