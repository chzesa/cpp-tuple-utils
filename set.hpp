#pragma once

#include <type_traits>
#include <tuple>

namespace tuple_utils
{

template <typename T>
struct SetLeaf {};

template <typename ...R>
struct SetImpl2;

template <typename V, typename ...R>
struct SetImpl2<V, R...> : SetLeaf<V>, SetImpl2<R...> { };

template <typename V>
struct SetImpl2<V> : SetLeaf<V> { };

template <>
struct SetImpl2<> {};

template <typename A, typename ...R>
struct SetImpl;

template <typename ...A, typename V, typename ...R>
struct SetImpl<SetImpl2<A...>, V, R...>
{
	using type = typename std::conditional<
		std::is_base_of<SetLeaf<V>, SetImpl2<A...>>() ? true : false
		, SetImpl<SetImpl2<A...>, R...>
		, SetImpl<SetImpl2<V, A...>, R...>
	>::type::type;
};

template <typename ...A, typename V>
struct SetImpl<SetImpl2<A...>, V>
{
	using type = typename std::conditional<
		std::is_base_of<SetLeaf<V>, SetImpl2<A...>>() ? true : false
		, std::tuple<A...>
		, std::tuple<V, A...>
	>::type;
};

template <typename V>
struct SetImpl<SetImpl2<>, V>
{
	using type = std::tuple<V>;
};

template <>
struct SetImpl<SetImpl2<>>
{
	using type = std::tuple<>;
};

template <typename ...Values>
using Set = typename SetImpl<SetImpl2<>, Values...>::type;

} // namespace tuple_utils
