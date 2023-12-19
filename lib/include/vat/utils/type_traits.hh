#pragma once

// STL
#include <concepts>

namespace vat::utils
{

template <template <typename...> typename T, typename Arg>
struct template_bind
{
	template <typename... Args>
	using type = T<Arg, Args...>;
};

template <template <typename...> typename T, template <typename> typename Pred, typename... Args>
struct template_call_except_impl;

template <template <typename...> typename T, template <typename> typename Pred, typename... Args>
using template_call_except_t = template_call_except_impl<T, Pred, Args...>::type;

template <template <typename...> typename T, template <typename> typename Pred>
struct template_call_except_impl<T, Pred>
{
	using type = T<>;
};

template <template <typename...> typename T, template <typename> typename Pred, typename Arg, typename... Args>
	requires Pred<Arg>::value
struct template_call_except_impl<T, Pred, Arg, Args...>
{
	using type = template_call_except_t<T, Pred, Args...>;
};

template <template <typename...> typename T, template <typename> typename Pred, typename Arg, typename... Args>
struct template_call_except_impl<T, Pred, Arg, Args...>
{
	using type = template_call_except_t<template_bind<T, Arg>::template type, Pred, Args...>;
};

template <template <typename> typename Pred>
struct template_call_except
{
	template <template <typename...> typename T>
	struct type_impl
	{
		template <typename... Args>
		using type = template_call_except_impl<T, Pred, Args...>;
	};

	template <template <typename...> typename T>
	using type = type_impl<T>;
};

template <typename Arg, typename... Args>
struct is_in_type_list;

template <typename Arg, typename... Args>
static constexpr bool is_in_type_list_v = is_in_type_list<Arg, Args...>::value;

template <typename Arg>
struct is_in_type_list<Arg>
{
	static constexpr bool value = false;
};

template <typename Arg, std::same_as<Arg> Arg1, typename... Args>
struct is_in_type_list<Arg, Arg1, Args...>
{
	static constexpr bool value = true;
};

template <typename Arg, typename Arg1, typename... Args>
struct is_in_type_list<Arg, Arg1, Args...>
{
	static constexpr bool value = is_in_type_list_v<Arg, Args...>;
};

template <template <typename...> typename T, typename... Args>
struct unique_template_call_impl;

template <template <typename...> typename T, typename... Args>
using unique_template_call_t = unique_template_call_impl<T, Args...>::type;

template <template <typename...> typename T>
struct unique_template_call_impl<T>
{
	using type = T<>;
};

template <template <typename...> typename T, typename Arg, typename... Args>
	requires is_in_type_list_v<Arg, Args...>
struct unique_template_call_impl<T, Arg, Args...>
{
	using type = unique_template_call_t<T, Args...>;
};

template <template <typename...> typename T, typename Arg, typename... Args>
struct unique_template_call_impl<T, Arg, Args...>
{
	using type = unique_template_call_t<template_bind<T, Arg>::template type, Args...>;
};

template <template <typename...> typename T>
struct unique_template_call
{
	template <typename... Args>
	using type = unique_template_call_t<T, Args...>;
};

} // namespace vat::utils
