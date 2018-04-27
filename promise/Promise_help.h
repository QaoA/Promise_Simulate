#include<utility>
#include<type_traits>
#include "NameSpace.h"

NAME_BEGIN

template <typename F, typename... Args>
using result_of = decltype(std::declval<F>()(std::declval<Args>()...));


template<typename F, typename...args>
struct callable_with
{
	template<typename T, typename = result_of<T, args...>>
	static std::true_type check(std::nullptr_t);

	template<typename T>
	static std::false_type check(...);

	typedef decltype(check<F>(nullptr)) type;
	static constexpr bool value = type::value;
};

template<bool callable, class F, typename...args>
struct is_func_legal_impl;

template<class F, typename...args>
struct is_func_legal_impl<true, F, args...>
{
	static constexpr bool value = std::is_same<void, result_of<F, args...>>::value;
};

template<class F, typename...args>
struct is_func_legal_impl<false, F, args...>
{
	static constexpr bool value = false;
};

template<class F, typename...args>
struct is_func_legal
{
	static bool const value = is_func_legal_impl<callable_with<F, args...>::value, F, args...>::value;
};

struct empty{};

template<typename R, typename F, typename ...Args>
inline R invoke_impl(std::false_type, std::false_type, F && func, Args&&...args)
{
	return func(std::forward<Args>(args)...);
}

template<typename R, typename F, typename ...Args>
inline R invoke_impl(std::false_type, std::true_type, F && func, Args&&...args)
{
	return func();
}

template<typename R, typename F, typename ...Args>
inline R invoke_impl(std::true_type, std::false_type, F && func, Args&&...args)
{
	func(std::forward<Args>(args)...);
	return empty{};
}

template<typename R, typename F, typename ...Args>
inline R invoke_impl(std::true_type, std::true_type, F && func, Args&&...args)
{
	func();
	return empty{};
}

template<bool, typename F, typename ...Args>
struct get_return_type;

template<typename F, typename ...Args>
struct get_return_type<true,F,Args...>
{
	typedef result_of<F> type;
};

template<typename F, typename ...Args>
struct get_return_type<false, F, Args...>
{
	typedef result_of<F, Args...> type;
};


template<typename F, typename ...Args>
struct invoker
{
	static const bool is_arg_void = !callable_with<F, Args...>::value;
	using FuncReturnType = typename get_return_type<is_arg_void, F, Args...>::type;
	static const bool is_ret_void = std::is_same<FuncReturnType, void>::value;
	using returnTag = std::integral_constant<bool, is_ret_void>;
	using argTag = std::integral_constant<bool, is_arg_void>;
	using R = std::conditional_t<is_ret_void, empty, FuncReturnType>;
	static inline R invoke(F && func, Args&&... args)
	{
		return invoke_impl<R>(returnTag{}, argTag{}, std::forward<F>(func), std::forward<Args>(args)...);
	}
};

NAME_END