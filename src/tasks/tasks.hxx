#pragma once

#include "tasks.hh"

// STL
#include <concepts>
#include <exception>
#include <future>

namespace vat
{

template <typename R, typename... Args>
template <typename F>
Task<R(Args...)>::Task(F&& f)
	: task_{ std::move(f) }
{}

template <typename R, typename... Args>
R Task<R(Args...)>::operator()(Args... args)
{
	std::future<R> future = task_.get_future();
	task_(args...);
	return future.get();
}

template <typename R, typename... Args>
std::future<R> Task<R(Args...)>::run(std::launch policy, Args... args)
{
	return std::async(policy, [args..., task = std::move(*this)]() { return task(args...); });
}

template <typename R, typename... Args>
template <typename R1>
Task<R1(Args...)> Task<R(Args...)>::and_then(Task<R1(R)> task)
{
	return Task<R1(Args...)>{ [task = std::move(task_), next = std::move(task)]() {
		if constexpr (std::same_as<R, void> && std::same_as<R1, void>)
			task(), next();
		else if constexpr (std::same_as<R1, void>)
			next(task());
		else if constexpr (std::same_as<R, void>)
			return task(), next();
		else
			return next(task());
	} };
}

template <typename R, typename... Args>
template <typename R1>
Task<utils::unique_variant<R, R1>(Args...)> Task<R(Args...)>::or_else(Task<R1(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R, R1>(Args...)>{ [task = std::move(*this), next = std::move(task)]() {
		try
		{
			return task();
		}
		catch (...)
		{
			return next(std::current_exception());
		}
	} };
}

template <typename R, typename... Args>
template <typename... R1>
Task<utils::unique_variant<R, R1...>(Args...)>
Task<R(Args...)>::or_else(Task<std::variant<R1...>(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R, R1...>(Args...)>{ [task = std::move(*this), next = std::move(task)]() {
		try
		{
			return task();
		}
		catch (...)
		{
			return std::visit([](auto&& value) -> utils::unique_variant<R, R1...> { return value; },
							  next(std::current_exception()));
		}
	} };
}

template <typename... R, typename... Args>
template <typename F>
Task<std::variant<R...>(Args...)>::Task(F&& f)
	: task_{ std::move(f) }
{}

template <typename... R, typename... Args>
std::variant<R...> Task<std::variant<R...>(Args...)>::operator()(Args... args)
{
	std::future<std::variant<R...>> future = task_.get_future();
	task_(args...);
	return future.get();
}

template <typename... R, typename... Args>
std::future<std::variant<R...>> Task<std::variant<R...>(Args...)>::run(std::launch policy, Args... args)
{
	return std::async(policy, [args..., task = std::move(*this)]() { return task(args...); });
}

template <typename... R, typename... Args>
template <typename R1>
Task<R1(Args...)> Task<std::variant<R...>(Args...)>::and_then(Task<R1(std::variant<R...>)> task)
{
	return Task<R1(Args...)>{ [task = std::move(task_), next = std::move(task)]() {
		if constexpr (std::same_as<R1, void>)
			next(task());
		else
			return next(task());
	} };
}

template <typename... R, typename... Args>
template <typename R1>
Task<utils::unique_variant<R..., R1>(Args...)>
Task<std::variant<R...>(Args...)>::or_else(Task<R1(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R..., R1>(Args...)>{ [task = std::move(*this), next = std::move(task)]() {
		try
		{
			return std::visit([](auto&& value) -> utils::unique_variant<R..., R1> { return value; }, task());
		}
		catch (...)
		{
			return next(std::current_exception());
		}
	} };
}

template <typename... R, typename... Args>
template <typename... R1>
Task<utils::unique_variant<R..., R1...>(Args...)>
Task<std::variant<R...>(Args...)>::or_else(Task<std::variant<R1...>(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R..., R1...>(Args...)>{ [task = std::move(*this), next = std::move(task)]() {
		try
		{
			return std::visit([](auto&& value) -> utils::unique_variant<R..., R1...> { return value; }, task());
		}
		catch (...)
		{
			return std::visit([](auto&& value) -> utils::unique_variant<R..., R1...> { return value; },
							  next(std::current_exception()));
		}
	} };
}

} // namespace vat
