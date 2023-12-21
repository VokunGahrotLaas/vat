#pragma once

#include <vat/utils/tasks.hh>

// STL
#include <concepts>
#include <exception>
#include <future>

namespace vat::utils
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
	return std::async<Task<R(Args...)>>(policy, std::move(*this), args...);
}

template <typename R, typename... Args>
template <typename R1>
Task<R1(Args...)> Task<R(Args...)>::and_then(Task<R1(R)> task)
{
	return Task<R1(Args...)>{ [task = std::move(*this), next = std::move(task)](Args... args) mutable -> R1 {
		if constexpr (std::same_as<R1, void>)
			next(task(args...));
		else
			return next(task(args...));
	} };
}

template <typename R, typename... Args>
template <typename R1>
Task<utils::unique_variant<R, R1>(Args...)> Task<R(Args...)>::or_else(Task<R1(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R, R1>(Args...)>{ [task = std::move(*this), next = std::move(task)](
															Args... args) mutable -> utils::unique_variant<R, R1> {
		try
		{
			return task(args...);
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
	return Task<utils::unique_variant<R, R1...>(Args...)>{
		[task = std::move(*this), next = std::move(task)](Args... args) mutable -> utils::unique_variant<R, R1...> {
			try
			{
				return task(args...);
			}
			catch (...)
			{
				return std::visit([](auto&& value) -> utils::unique_variant<R, R1...> { return value; },
								  next(std::current_exception()));
			}
		}
	};
}

template <typename... Args>
template <typename F>
Task<void(Args...)>::Task(F&& f)
	: task_{ std::move(f) }
{}

template <typename... Args>
void Task<void(Args...)>::operator()(Args... args)
{
	std::future<void> future = task_.get_future();
	task_(args...);
	future.get();
}

template <typename... Args>
std::future<void> Task<void(Args...)>::run(std::launch policy, Args... args)
{
	return std::async(policy, [args..., task = std::move(*this)]() mutable -> void { task(args...); });
}

template <typename... Args>
template <typename R1>
Task<R1(Args...)> Task<void(Args...)>::and_then(Task<R1()> task)
{
	return Task<R1(Args...)>{ [task = std::move(*this), next = std::move(task)](Args... args) mutable -> R1 {
		if constexpr (std::same_as<R1, void>)
			task(args...), next();
		else
			return task(args...), next();
	} };
}

template <typename... Args>
template <typename R1>
Task<utils::unique_variant<void, R1>(Args...)> Task<void(Args...)>::or_else(Task<R1(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<void, R1>(Args...)>{
		[task = std::move(*this), next = std::move(task)](Args... args) mutable -> utils::unique_variant<void, R1> {
			try
			{
				task(args...);
				return utils::unique_variant<void, R1>{};
			}
			catch (...)
			{
				if constexpr (std::same_as<R1, void>)
				{
					next(std::current_exception());
					return utils::unique_variant<void, R1>{};
				}
				else
					return next(std::current_exception());
			}
		}
	};
}

template <typename... Args>
template <typename... R1>
Task<utils::unique_variant<void, R1...>(Args...)>
Task<void(Args...)>::or_else(Task<std::variant<R1...>(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<void, R1...>(Args...)>{
		[task = std::move(*this), next = std::move(task)](Args... args) mutable -> utils::unique_variant<void, R1...> {
			try
			{
				task(args...);
				return utils::unique_variant<void, R1...>{};
			}
			catch (...)
			{
				return std::visit([](auto&& value) -> utils::unique_variant<void, R1...> { return value; },
								  next(std::current_exception()));
			}
		}
	};
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
	return std::async(policy,
					  [args..., task = std::move(*this)]() mutable -> std::variant<R...> { return task(args...); });
}

template <typename... R, typename... Args>
template <typename R1>
Task<R1(Args...)> Task<std::variant<R...>(Args...)>::and_then(Task<R1(std::variant<R...>)> task)
{
	return Task<R1(Args...)>{ [task = std::move(*this), next = std::move(task)](Args... args) mutable -> R1 {
		if constexpr (std::same_as<R1, void>)
			next(task(args...));
		else
			return next(task(args...));
	} };
}

template <typename... R, typename... Args>
template <typename R1>
Task<utils::unique_variant<R..., R1>(Args...)>
Task<std::variant<R...>(Args...)>::or_else(Task<R1(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R..., R1>(Args...)>{
		[task = std::move(*this), next = std::move(task)](Args... args) mutable -> utils::unique_variant<R..., R1> {
			try
			{
				return std::visit([](auto&& value) -> utils::unique_variant<R..., R1> { return value; }, task(args...));
			}
			catch (...)
			{
				if constexpr (std::same_as<R1, void>)
				{
					next(std::current_exception());
					return utils::unique_variant<R..., R1>{};
				}
				else
					return next(std::current_exception());
			}
		}
	};
}

template <typename... R, typename... Args>
template <typename... R1>
Task<utils::unique_variant<R..., R1...>(Args...)>
Task<std::variant<R...>(Args...)>::or_else(Task<std::variant<R1...>(std::exception_ptr)> task)
{
	return Task<utils::unique_variant<R..., R1...>(Args...)>{
		[task = std::move(*this), next = std::move(task)](Args... args) mutable -> utils::unique_variant<R..., R1...> {
			try
			{
				return std::visit([](auto&& value) -> utils::unique_variant<R..., R1...> { return value; },
								  task(args...));
			}
			catch (...)
			{
				return std::visit([](auto&& value) -> utils::unique_variant<R..., R1...> { return value; },
								  next(std::current_exception()));
			}
		}
	};
}

} // namespace vat::utils
