#pragma once

#include "recursion.hpp"
#include "iteration.hpp"
#include <iostream>
#include <typeinfo>

namespace tuple_utils
{

/*
Edges is a tuple of tuples
std::tuple<
	std::tuple<A, B>, // Implies A -> B
	std::tuple<B, C> // Implies B -> C
>;
makes a graph with 3 nodes, where A depends on B
and B depends on C. I.e. A is the child of B.
Must be acyclic
*/

template <typename Edges>
struct Graph
{
// private:
	struct DependencyCallback
	{
		template <typename T, typename F>
		static void callback(F f)
		{
			f.template operator()<T>();
		}
	};

	template <typename B>
	struct TransitiveDependencyTest
	{
		template <typename T>
		static constexpr bool callback()
		{
			return dependsOn<T, B>();
		}
	};

	template <typename A, size_t I>
	struct FilterI
	{
		template <typename T>
		static constexpr bool test()
		{
			return std::is_same<typename std::tuple_element<I, T>::type, A >::value;
		}
	};

	template <size_t I>
	struct MapperI
	{
		template <typename T>
		using type = std::tuple_element<I, T>::type;
	};

	// finds all * s.t. exists edge <N, *>
	template <typename N>
	struct DirectDependees
	{
		using n_out_edges = Subset< Edges, FilterI<N, 0> >;
		using type = Map< n_out_edges, MapperI<1> >;
	};

	// finds all * s.t. exists edge <*, N>
	template <typename N>
	struct DirectDependants
	{
		using n_in_edges = Subset<Edges, FilterI<N, 1>>;
		using type = Map< n_in_edges, MapperI<0> >;
	};

// public:
	using Nodes = Union<Map<Edges, MapperI<0>>, Map<Edges, MapperI<1>>>;

	template <typename A, typename B>
	static constexpr bool dependsOn()
	{

		return directlyDependsOn<A, B>() || OncePerType< typename DirectDependees<A>::type, TransitiveDependencyTest<B> >::constFn();
	}

	// Tests A -> B
	template <typename A, typename B>
	static constexpr bool directlyDependsOn()
	{
		return Contains<typename DirectDependants<B>::type, A>::value;
	}

	template <typename A, typename F>
	static inline void forDependants2(F f)
	{
		OncePerType<Subset<Nodes, DependsOnFilter<A>>, DependencyCallback>::fn(f);
	}

	template <typename A, typename F>
	static inline void forDirectDependants2(F f)
	{
		OncePerType<Subset<Nodes, DirectlyDependsOnFilter<A>>, DependencyCallback>::fn(f);
	}

	// template <typename A>
	// static constexpr uint64_t numDependees()
	// {
	// 	return 0; // todo
	// }

	// template <typename A>
	// static constexpr uint64_t numDependencies()
	// {
	// 	return std::tuple_size<Set<typename Dependencies<A>::type>>::value;
	// }

private:

	template <typename A>
	struct DependsOnFilter
	{
		template <typename T>
		static constexpr bool test()
		{
			return dependsOn<T, A>();
		}
	};

	template <typename A>
	struct DirectlyDependsOnFilter
	{
		template <typename T>
		static constexpr bool test()
		{
			return directlyDependsOn<T, A>();
		}
	};

	
};

} // namespace tuple_utils
