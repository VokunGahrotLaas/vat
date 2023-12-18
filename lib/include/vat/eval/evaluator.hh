#pragma once

namespace vat::eval
{

template <typename Input, typename Exp>
class Evaluator
{
public:
	using input_type = Input;
	using exp_type = Exp;

	virtual exp_type eval(input_type input) = 0;
	virtual void reset() = 0;
};

} // namespace vat::eval
