#pragma once

// STL
#include <variant>

// vat
#include <vat/utils/type_traits.hh>

namespace vat::utils
{

template <typename T>
struct is_void
{
	static constexpr bool value = std::same_as<T, void>;
};

template <typename T>
struct is_void_or_monostate
{
	static constexpr bool value = std::same_as<T, void> || std::same_as<T, std::monostate>;
};

template <template <typename...> typename T, typename... Args>
struct template_call_void_to_monostate_impl
{
	using type = T<Args...>;
};

template <template <typename...> typename T, typename... Args>
	requires is_in_type_list_v<void, Args...>
struct template_call_void_to_monostate_impl<T, Args...>
{
	using type = template_call_except_t<template_bind<T, std::monostate>::template type, is_void_or_monostate, Args...>;
};

template <template <typename...> typename T>
struct template_call_void_to_monostate
{
	template <typename... Args>
	using type = template_call_void_to_monostate_impl<T, void, Args...>::type;
};

template <typename... Args>
using unique_variant =
	unique_template_call<template_call_void_to_monostate<std::variant>::template type>::template type<Args...>;

template <typename T, typename... Ts>
struct overloads
	: T
	, Ts...
{
	using T::T;
	using T::operator(), Ts::operator()...;
};

} // namespace vat::utils
