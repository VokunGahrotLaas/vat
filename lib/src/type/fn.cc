#include <vat/type/fn.hh>

namespace vat::type
{

Fn::Fn(SharedConstType return_type, std::vector<SharedConstType> args)
	: return_type_{ std::move(return_type) }
	, args_{ std::move(args) }
{}

std::ostream& Fn::print(std::ostream& os) const
{
	os << "fn (";
	auto it = args_.begin();
	if (it != args_.end()) os << **(it++);
	for (; it != args_.end(); ++it)
		os << ", " << **it;
	return os << ") -> " << *return_type_;
}

} // namespace vat::type
