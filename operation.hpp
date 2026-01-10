#pragma once

#include <tuple>
#include "set.hpp"

namespace tuple_utils
{

template <typename Tuple, typename ...R>
struct UnionImpl;

template <typename ...A, typename ...B, typename ...R>
struct UnionImpl<std::tuple<A...>, std::tuple<B...>, R...>
{
	using type = typename UnionImpl<std::tuple<A..., B...>, R...>::type;
};

template <typename ...A, typename ...B>
struct UnionImpl<std::tuple<A...>, std::tuple<B...>>
{
	using type = Set<A..., B...>;
};

template <typename ...V>
using Union = typename UnionImpl<V...>::type;

template <typename V, typename T>
struct ContainsImpl;

template <typename V>
struct ContainsImpl<V, std::tuple<>> : std::false_type {};

template <typename V, typename A, typename ...R>
struct ContainsImpl<V, std::tuple<A, R...>> : ContainsImpl<V, std::tuple<R...>> {};

template <typename V, typename ...R>
struct ContainsImpl<V, std::tuple<V, R...>> : std::true_type {};

template <typename Tuple, typename V>
struct Contains
{
	static constexpr bool value = ContainsImpl<V, Tuple>::value;
};

template <typename V, typename Tuple>
struct IndexImpl;

template <typename V, typename ...R>
struct IndexImpl<V, std::tuple<V, R...>>
{
	static const std::size_t value = 0;
};

template <typename V, typename A, typename ...R>
struct IndexImpl<V, std::tuple<A, R...>>
{
	static const std::size_t value = 1 + IndexImpl<V, std::tuple<R...>>::value;
};

template <typename V>
struct IndexImpl<V, std::tuple<>>
{
	static const std::size_t value = 0;
};

template <typename V, typename Tuple>
struct Index
{
	static constexpr std::size_t value = IndexImpl<V, Tuple>::value;
};

template <typename A, typename B, typename ...R>
struct DifferenceImpl;

template <typename ...A, typename B, typename V, typename ...R>
struct DifferenceImpl<std::tuple<A...>, B, V, R...>
{
	using type = typename std::conditional<
		Contains<B, V>::value
		, DifferenceImpl<std::tuple<A...>, B, R...>
		, DifferenceImpl<std::tuple<V, A...>, B, R...>
	>::type::type;
};

template <typename ...A, typename B, typename V>
struct DifferenceImpl<std::tuple<A...>, B, V>
{
	using type = typename std::conditional<
		Contains<B, V>::value
		, Set<A...>
		, Set<V, A...>
	>::type;
};

template <typename ...A, typename B>
struct DifferenceImpl<std::tuple<A...>, B>
{
	using type = std::tuple<>;
};

template <typename A, typename B>
struct DifferenceImpl2;

template <typename A, typename ...B>
struct DifferenceImpl2<A, std::tuple<B...>>
{
	using type = typename DifferenceImpl<std::tuple<>, A, B...>::type;
};

template <typename A, typename B>
using Difference = typename DifferenceImpl2<B, A>::type;

template <typename A, typename B>
using SymmetricDifference = Union<Difference<Union<A, B>, A>, Difference<Union<A, B>, B>>;

template <typename A, typename B>
using Intersect = Difference<Union<A, B>, SymmetricDifference<A, B>>;

template <typename Tuple, typename B, typename ...R>
struct CartesianProductImpl;

template <typename ...A, typename B, typename V, typename ...R>
struct CartesianProductImpl<std::tuple<A...>, B, V, R...>
{
	using type = typename CartesianProductImpl<std::tuple<std::tuple<B, V>, A...>, B, R...>::type;
};

template <typename ...A, typename B, typename V>
struct CartesianProductImpl<std::tuple<A...>, B, V>
{
	using type = std::tuple<std::tuple<B, V>, A...>;
};

template <typename A, typename B, typename ...R>
struct CartesianProductImpl2;

template <typename ...A, typename ...B, typename V, typename ...R>
struct CartesianProductImpl2<std::tuple<A...>, std::tuple<B...>, V, R...>
{
	using type = typename CartesianProductImpl2<typename CartesianProductImpl<std::tuple<A...>, V, B...>::type, std::tuple<B...>, R... >::type;
};

template <typename ...A, typename ...B, typename V>
struct CartesianProductImpl2<std::tuple<A...>, std::tuple<B...>, V>
{
	using type = typename CartesianProductImpl<std::tuple<A...>, V, B...>::type;
};

template <typename A, typename B>
struct CartesianProductImpl_3;

template <typename ...A, typename ...B>
struct CartesianProductImpl_3<std::tuple<A...>, std::tuple<B...>>
{
	using type = typename CartesianProductImpl2<std::tuple<>, std::tuple<B...>, A...>::type;
};

template <typename A, typename B>
using CartesianProduct = typename CartesianProductImpl_3<A, B>::type;

template <typename F, typename A, typename ...R>
struct SubsetImpl;

template <typename F, typename ...A, typename V, typename ...R>
struct SubsetImpl<F, std::tuple<A...>, V, R...>
{
	using type = typename std::conditional<
		F::template test<V>()
		, SubsetImpl<F, std::tuple<V, A...>, R...>
		, SubsetImpl<F, std::tuple<A...>, R...>
	>::type::type;
};

template <typename F, typename ...A, typename V>
struct SubsetImpl<F, std::tuple<A...>, V>
{
	using type = typename std::conditional<
		F::template test<V>()
		, Set<V, A...>
		, Set<A...>
	>::type;
};

template <typename T, typename F>
struct SubsetImpl_2;

template <typename ...A, typename F>
struct SubsetImpl_2<std::tuple<A...>, F>
{
	using type = typename SubsetImpl<F, std::tuple<>, A...>::type;
};

template <typename F>
struct SubsetImpl_2<std::tuple<>, F>
{
	using type = std::tuple<>;
};

template<typename Tuple, typename F>
using Subset = typename SubsetImpl_2<Tuple, F>::type;

template <typename A, typename M, typename ...R>
struct MapImpl;

template <typename ...A, typename M, typename V, typename ...R>
struct MapImpl<std::tuple<A...>, M, V, R...>
{
	using type = typename MapImpl<std::tuple<A..., typename M::type<V>>, M, R...>::type;
};

template <typename ...A, typename M, typename V>
struct MapImpl<std::tuple<A...>, M, V>
{
	using type = std::tuple<A..., typename M::type<V>>;
};

template <typename ...A, typename M>
struct MapImpl<std::tuple<A...>, M>
{
	using type = std::tuple<A...>;
};

template <typename M, typename A>
struct MapImpl2;

template <typename M, typename ...A>
struct MapImpl2<M, std::tuple<A...>>
{
	using type = typename MapImpl<std::tuple<>, M, A...>::type;
};

template <typename Tuple, typename M>
using Map = typename MapImpl2<M, Tuple>::type;

} //namespace tuple_utils
