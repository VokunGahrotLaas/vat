#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(binding_assets, FailOn::Binding);
	code |= tests_on(typing_assets, SuccessUntil::Binding);
	code |= tests_on(good_assets, SuccessUntil::Binding);

	return code;
}
