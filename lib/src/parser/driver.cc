#include <vat/parser/driver.hh>

// STL
#include <fstream>
#include <iostream>

// vat
#include <vat/parser/lexer.hh>
#include <vat/parser/parser.yy.hh>

namespace vat::parser
{

Driver::Driver(std::string_view filename, bool trace_parsing, bool trace_scanning)
	: filename_{ filename }
	, trace_parsing_(trace_parsing)
	, trace_scanning_(trace_scanning)
{
	loc_.initialize(&filename_);
}

Driver::~Driver() {}

bool Driver::parse()
{
	if (finished_) return success_;
	is_ = &std::cin;
	os_ = &std::cerr;
	if (!filename_.empty() && filename_ != "-")
	{
		is_ = new std::ifstream{ filename_ };
		if (!*is_) throw std::ios::failure("failed to open file " + filename_);
	}
	lexer_.reset(new Lexer{ *is_, *os_ });
	lexer_->set_debug(trace_scanning_);
	yyParser parser{ *this };
	parser.set_debug_level(trace_parsing_);
	success_ = parser() == 0;
	lexer_.reset();
	if (is_ != &std::cin) delete is_;
	is_ = &std::cin;
	finished_ = true;
	return success_;
}

} // namespace vat::parser
