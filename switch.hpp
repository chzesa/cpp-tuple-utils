#pragma once

#include <tuple>

namespace tuple_utils
{

template <typename Tuple, int Num>
struct SwitchImpl
{
	template <typename Return, typename Inspector, typename ...Params>
	constexpr static Return fn(int i, Params&&... params)
	{
		return i == Num - 1
			? Inspector::template callback<typename std::tuple_element<Num - 1, Tuple>::type>(std::forward<Params>(params)...)
			: SwitchImpl<Tuple, Num - 1>::template fn<Return, Inspector>(i, std::forward<Params>(params)...);
	}

	template <typename Inspector, typename ...Params>
	inline static void fn(int i, Params&&... params)
	{
		if (i == Num - 1)
			Inspector::template callback<typename std::tuple_element<Num - 1, Tuple>::type>(std::forward<Params>(params)...);
		else
			SwitchImpl<Tuple, Num - 1>::template fn<Inspector>(i, std::forward<Params>(params)...);
	}

	template <typename Inspector, typename F>
	inline static void fn2(int i, F f)
	{
		if (i == Num - 1)
			f.template operator() <typename std::tuple_element<Num - 1, Tuple>::type>();
		else
			SwitchImpl<Tuple, Num - 1>::template fn2<Inspector, F>(i, f);
	}
};

template <typename Tuple>
struct SwitchImpl <Tuple, 0>
{
	template <typename Return,  typename Inspector, typename ...Params>
	constexpr static Return fn(int i, Params&&... params) { return Return(); }

	template <typename Inspector, typename ...Params>
	inline static void fn(int i, Params&&... params) { }

	template <typename Inspector, typename F>
	inline static void fn2(int i, F f) { }
};

template <typename Tuple>
struct Switch
{
	template <typename Return, typename Inspector, typename ...Params>
	constexpr static Return fn(int i, Params&&... params)
	{
		return SwitchImpl<Tuple, std::tuple_size<Tuple>::value>::template fn<Return, Inspector>(i, std::forward<Params>(params)...);
	}

	template <typename Inspector, typename ...Params>
	inline static void fn(int i, Params&&... params)
	{
		SwitchImpl<Tuple, std::tuple_size<Tuple>::value>::template fn<Inspector>(i, std::forward<Params>(params)...);
	}

	template <typename Inspector, typename F>
	inline static void fn2(int i, F f)
	{
		SwitchImpl<Tuple, std::tuple_size<Tuple>::value>::template fn<Inspector>(i, f);
	}
};

} // namespace tuple_utils
