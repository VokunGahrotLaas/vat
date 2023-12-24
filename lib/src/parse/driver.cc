#include <vat/parse/driver.hh>

// STL
#include <fstream>
#include <iostream>

// vat
#include <vat/parse/lexer.hh>
#include <vat/parse/parser.yy.hh>

namespace vat::parse
{

Driver::Driver(std::string_view filename, utils::ErrorManager& em, bool trace_parsing, bool trace_scanning)
	: filename_{ filename }
	, error_{ em }
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
		if (!*is_)
		{
			parse::location loc{ &filename_ };
			error_.error(utils::ErrorType::Other, loc) << "no such file or directory";
			success_ = false;
			finished_ = true;
			return false;
		}
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

} // namespace vat::parse
