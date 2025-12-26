#pragma once

#include "recursion.hpp"
#include "iteration.hpp"

namespace tuple_utils
{

template <template <typename> class Dependencies, typename ...Nodes>
struct Graph
{
	template <typename A, typename B>
	static constexpr bool transitivelyDependsOn()
	{
		return Contains<Flatten<Dependencies, Expand<Dependencies, typename Dependencies<A>::type>>, B>::value;
	}

	template <typename A, typename B>
	static constexpr bool directlyDependsOn()
	{
		return Contains<typename Dependencies<A>::type, B>::value
			&& !transitivelyDependsOn<A, B>();
	}

	template <typename A, typename B>
	static constexpr bool dependsOn()
	{
		return directlyDependsOn<A, B>() || transitivelyDependsOn<A, B>();
	}

	// template <typename A, typename F>
	// static inline void forDependants2(F f)
	// {
	// 	OncePerType<Subset<Set<Nodes...>, DependsOnFilter<A>>, DependencyCallback>::fn(f);
	// }

	// template <typename A, typename F>
	// static inline void forDirectDependants2(F f)
	// {
	// 	OncePerType<Subset<Set<Nodes...>, DirectlyDependsOnFilter<A>>, DependencyCallback>::fn(f);
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

	template <typename>
	struct DependencyCallback
	{
		template <typename T, typename F>
		static void callback(F f)
		{
			f.template operator()<T>();
		}
	};
};

} // namespace tuple_utils
