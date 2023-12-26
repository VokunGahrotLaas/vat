#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(lex_assets, FailOn::Lexing);
	code |= tests_on(parse_assets, SuccessUntil::Lexing);
	code |= tests_on(bind_assets, SuccessUntil::Lexing);
	code |= tests_on(type_assets, SuccessUntil::Lexing);
	code |= tests_on(eval_assets, SuccessUntil::Lexing);
	code |= tests_on(good_assets, SuccessUntil::Lexing);

	return code;
}
