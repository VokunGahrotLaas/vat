#pragma once

// STL
#include <future>

// vat
#include <type_traits>
#include <vat/utils/variant.hh>

namespace vat
{

template <typename>
class Task;

template <typename R, typename... Args>
class Task<R(Args...)>
{
public:
	template <typename F>
	explicit Task(F&& f);

	R operator()(Args... args);

	std::future<R> run(std::launch policy, Args... args);

	template <typename R1>
	Task<R1(Args...)> and_then(Task<R1(R)> task);

	template <typename R1>
	Task<utils::unique_variant<R, R1>(Args...)> or_else(Task<R1(std::exception_ptr)> task);

	template <typename... R1>
	Task<utils::unique_variant<R, R1...>(Args...)> or_else(Task<std::variant<R1...>(std::exception_ptr)> task);

private:
	std::packaged_task<R(Args...)> task_;
};

template <typename... R, typename... Args>
class Task<std::variant<R...>(Args...)>
{
public:
	template <typename F>
	explicit Task(F&& f);

	std::variant<R...> operator()(Args... args);

	std::future<std::variant<R...>> run(std::launch policy, Args... args);

	template <typename R1>
	Task<R1(Args...)> and_then(Task<R1(std::variant<R...>)> task);

	template <typename R1>
	Task<utils::unique_variant<R..., R1>(Args...)> or_else(Task<R1(std::exception_ptr)> task);

	template <typename... R1>
	Task<utils::unique_variant<R..., R1...>(Args...)> or_else(Task<std::variant<R1...>(std::exception_ptr)> task);

private:
	std::packaged_task<std::variant<R...>(Args...)> task_;
};

template <typename... Args>
auto make_task(auto&& f)
{
	return Task<std::invoke_result_t<std::decay_t<decltype(f)>, Args...>(Args...)>{ std::move(f) };
}

} // namespace vat

#include "tasks.hxx"
