#include "test_utils.hh"

int main()
{
	int code = 0;

	code |= tests_on(eval_assets, FailOn::Evaluation);
	code |= tests_on(good_assets, SuccessUntil::Evaluation);

	return code;
}
