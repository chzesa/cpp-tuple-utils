#pragma once
#include <cstddef>
#include <cstdint>
#include <tuple>

namespace tuple_utils
{

template <size_t I, typename T>
struct ForEach
{
	inline static void fn()
	{
		T::template callback<I>();
		ForEach<I - 1, T>::fn();
	}

	template<typename ...Params>
	inline static void fn(Params&&... params)
	{
		T::template callback<I>(std::forward<Params>(params)...);
		ForEach<I - 1, T>::fn(std::forward<Params>(params)...);
	}

	constexpr static bool constFn()
	{
		return T::template callback<I>() || ForEach<I - 1, T>::constFn();
	}

	template<typename ...Params>
	constexpr static bool constFn(Params&&... params)
	{
		return T::template callback<I>(std::forward<Params>(params)...) || ForEach<I - 1, T>::constFn(std::forward<Params>(params)...);
	}

	constexpr static size_t constSum()
	{
		return T::template callback<I>() + ForEach<I - 1, T>::constFn();
	}
};

template <typename T>
struct ForEach<0, T>
{
	inline static void fn()
	{
		T::template callback<0>();
	}

	template<typename ...Params>
	inline static void fn(Params&&... params)
	{
		T::template callback<0>(std::forward<Params>(params)...);
	}

	constexpr static bool constFn()
	{
		return T::template callback<0>() || false;
	}

	template<typename ...Params>
	constexpr static bool constFn(Params&&... params)
	{
		return T::template callback<0>(std::forward<Params>(params)...) || false;
	}

	constexpr static size_t constSum()
	{
		return T::template callback<0>();
	}
};

template <typename T>
struct ForEach<uint64_t(-1), T>
{
	inline static void fn() { }

	template<typename ...Params>
	inline static void fn(Params&&... params) { }

	constexpr static bool constFn() { return false; }

	template<typename ...Params>
	constexpr static bool constFn(Params&&... params) { return false; }

};

template <typename Tuple, typename Callback>
struct OncePerType
{
	inline static void fn()
	{
		ForEach<std::tuple_size<Tuple>::value - 1, CB>::fn();
	}

	template <typename ...Params>
	inline static void fn(Params&&... params)
	{
		ForEach<std::tuple_size<Tuple>::value - 1, CB>::fn(std::forward<Params>(params)...);
	}

	constexpr static bool constFn()
	{
		return ForEach<std::tuple_size<Tuple>::value - 1, CB2<bool>>::constFn();
	}

	template <typename ...Params>
	constexpr static bool constFn(Params&&... params)
	{
		return ForEach<std::tuple_size<Tuple>::value - 1, CB2<bool>>::constFn(std::forward<Params>(params)...);
	}

	constexpr static size_t constSum()
	{
		return ForEach<std::tuple_size<Tuple>::value - 1, CB2<size_t>>::constFn();
	}

private:
	struct CB
	{
		template <size_t I>
		inline static void callback()
		{
			Callback::template callback<typename std::tuple_element<I, Tuple>::type>();
		}

		template <size_t I, typename ...Params>
		inline static void callback(Params&&... params)
		{
			Callback::template callback<typename std::tuple_element<I, Tuple>::type>(std::forward<Params>(params)...);
		}
	};

	template <typename Ret>
	struct CB2
	{
		template <size_t I>
		constexpr static Ret callback()
		{
			return Callback::template callback<typename std::tuple_element<I, Tuple>::type>();
		}

		template <size_t I, typename ...Params>
		constexpr static Ret callback(Params&&... params)
		{
			return Callback::template callback<typename std::tuple_element<I, Tuple>::type>(std::forward<Params>(params)...);
		}
	};
};

template <typename Tuple>
struct OncePerType2Cb
{
	template <size_t I, typename F>
	static void callback(F f)
	{
		f.template operator()<typename std::tuple_element<I, Tuple>::type>();
	}
};

template <typename Tuple, typename F>
void oncePerType2(F f)
{
	ForEach<std::tuple_size<Tuple>::value - 1, OncePerType2Cb<Tuple>>::fn(f);
}

} // namespace tuple_utils
