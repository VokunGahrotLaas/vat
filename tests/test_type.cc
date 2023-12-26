#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(type_assets, FailOn::Typing);
	code |= tests_on(eval_assets, SuccessUntil::Typing);
	code |= tests_on(good_assets, SuccessUntil::Typing);

	return code;
}
