#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(lexing_assets, FailOn::Lexing);
	code |= tests_on(parsing_assets, SuccessUntil::Lexing);
	code |= tests_on(binding_assets, SuccessUntil::Lexing);
	code |= tests_on(typing_assets, SuccessUntil::Lexing);
	code |= tests_on(good_assets, SuccessUntil::Lexing);

	return code;
}
