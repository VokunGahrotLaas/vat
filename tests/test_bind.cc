#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(bind_assets, FailOn::Binding);
	code |= tests_on(type_assets, SuccessUntil::Binding);
	code |= tests_on(eval_assets, SuccessUntil::Binding);
	code |= tests_on(good_assets, SuccessUntil::Binding);

	return code;
}
