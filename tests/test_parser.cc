#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(parsing_assets, FailOn::Parsing);
	code |= tests_on(binding_assets, SuccessUntil::Parsing);
	code |= tests_on(typing_assets, SuccessUntil::Parsing);
	code |= tests_on(good_assets, SuccessUntil::Parsing);

	return code;
}
