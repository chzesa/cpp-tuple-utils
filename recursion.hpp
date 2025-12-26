#pragma once

#include <tuple>
#include "operation.hpp"

namespace tuple_utils
{

template <template<typename> class Extractor, typename T>
struct FlattenImpl2;

template <template<typename> class Extractor, typename Tuple>
using Flatten = typename FlattenImpl2<Extractor, Tuple>::type;

template <template<typename> class Extractor, typename A, typename ...R>
struct FlattenImpl;

template <template<typename> class Extractor, typename ...A, typename V, typename ...R>
struct FlattenImpl<Extractor, std::tuple<A...>, V, R...>
{
	using type = typename FlattenImpl<Extractor, Union<std::tuple<A...>, std::tuple<V>, Flatten<Extractor, typename Extractor<V>::type>>, R...>::type;
};

template <template<typename> class Extractor, typename ...A, typename V>
struct FlattenImpl<Extractor, std::tuple<A...>, V>
{
	using type = Union<std::tuple<A...>, std::tuple<V>, Flatten<Extractor, typename Extractor<V>::type>>;
};

template <template<typename> class Extractor, typename ...A>
struct FlattenImpl2<Extractor, std::tuple<A...>>
{
	using type = typename FlattenImpl<Extractor, std::tuple<>, A...>::type;
};

template <template<typename> class Extractor>
struct FlattenImpl2<Extractor, std::tuple<>>
{
	using type = std::tuple<>;
};

template <template<typename> class Extractor, typename A, typename ...R>
struct ExpandImpl;

template <template<typename> class Extractor, typename ...A, typename V, typename ...R>
struct ExpandImpl<Extractor, std::tuple<A...>, V, R...>
{
	using type = typename ExpandImpl<Extractor, Union<typename Extractor<V>::type, std::tuple<A...>>, R...>::type;
};

template <template<typename> class Extractor, typename ...A, typename V>
struct ExpandImpl<Extractor, std::tuple<A...>, V>
{
	using type = Union<typename Extractor<V>::type, std::tuple<A...>>;
};

template <template<typename> class Extractor, typename A>
struct ExpandImpl2;

template <template<typename> class Extractor, typename ...A>
struct ExpandImpl2<Extractor, std::tuple<A...>>
{
	using type = typename ExpandImpl<Extractor, std::tuple<>, A...>::type;
};

template <template<typename> class Extractor>
struct ExpandImpl2<Extractor, std::tuple<>>
{
	using type = std::tuple<>;
};

template <template<typename> class Extractor, typename Tuple>
using Expand = typename ExpandImpl2<Extractor, Tuple>::type;

} // namespace tuple_utils
