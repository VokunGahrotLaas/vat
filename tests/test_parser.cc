#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(parse_assets, FailOn::Parsing);
	code |= tests_on(bind_assets, SuccessUntil::Parsing);
	code |= tests_on(type_assets, SuccessUntil::Parsing);
	code |= tests_on(eval_assets, SuccessUntil::Parsing);
	code |= tests_on(good_assets, SuccessUntil::Parsing);

	return code;
}
