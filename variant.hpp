#pragma once

#include <utility>
#include <cstddef>
#include <tuple>

namespace tuple_utils
{

template <typename Value, typename ...Rest>
struct VariantImpl
{
	template <typename T>
	static constexpr T max(T a, T b)
	{
		return a > b ? a : b;
	}

	static constexpr size_t alignOf()
	{
		return max(alignof(Value), VariantImpl<Rest...>::alignOf());
	}

	template <typename V>
	static constexpr size_t keyOf()
	{
		return std::is_same<V, Value>() ? sizeof...(Rest) : VariantImpl<Rest...>::template keyOf<V>();
	}

	static constexpr size_t sizeOf()
	{
		return max(sizeof(Value), VariantImpl<Rest...>::sizeOf());
	}

	static void destroy(const size_t& i, void* p)
	{
		if (i == keyOf<Value>())
			reinterpret_cast<Value*>(p)->~Value();
		else
			VariantImpl<Rest...>::destroy(i, p);
	}

	template <typename T, typename ...Params>
	static void switchf(const size_t& key, Params&&... params)
	{
		if (keyOf<Value>() == key)
			T::template callback<Value>(std::forward<Params>(params)...);
		else
			VariantImpl<Rest...>::template switchf<T>(key, std::forward<Params>(params)...);
	}

	template <typename F, typename T>
	void switchf(const size_t& key, F&& f, T& t)
	{
		if (keyOf<Value>() == key)
			f(t.template get<T>());
		else
			VariantImpl<Rest...>::template switchf<T>(std::forward(f), t);
	}
};

template <typename Value>
struct VariantImpl <Value>
{
	static constexpr size_t alignOf()
	{
		return alignof(Value);
	}

	template <typename V>
	static constexpr size_t keyOf()
	{
		return std::is_same<V, Value>() ? 0 : -1;
	}

	static constexpr size_t sizeOf()
	{
		return sizeof(Value);
	}

	static void destroy(const size_t& i, void* p)
	{
		if (i == 0)
			reinterpret_cast<Value*>(p)->~Value();
	}

	template <typename T, typename ...Params>
	static void switchf(const size_t key, Params&&... params)
	{
		if (keyOf<Value>() == key)
			T::template callback<Value>(std::forward<Params>(params)...);
	}

	template <typename F, typename T>
	void switchf(const size_t& key, F&& f, T& t)
	{
		if (keyOf<Value>() == key)
			f(t.template get<T>());
	}
};

template <typename ...Values>
struct alignas(VariantImpl<Values...>::alignOf() * sizeof(size_t)) Variant
{
	static constexpr size_t size()
	{
		return sizeof...(Values);
	}

	Variant()
	{
		_key = -1;
	}

	template <typename V>
	explicit Variant(const V& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		_key = keyOf<V>();
		*(reinterpret_cast<V*>(data)) = v;
	}

	template <typename V>
	explicit Variant(V&& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		_key = keyOf<V>();
		*(reinterpret_cast<typename std::remove_reference<V>::type*>(data)) = v;
	}

	~Variant()
	{
		VariantImpl<Values...>::destroy(_key, data);
	}

	template <typename F>
	void switchf(F&& f)
	{
		VariantImpl<Values...>::template switchf(_key, f, *this);
	}

	template <typename T, typename ...Params>
	void switchf(Params&&... params)
	{
		VariantImpl<Values...>::template switchf<T>(_key, std::forward<Params>(params)...);
	}

	template <typename V, typename ...Params>
	static Variant construct(Params&&... params)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		Variant ret;
		ret._key = keyOf<V>();
		new (reinterpret_cast<V*>(ret.data)) V(std::forward<Params>(params)...);
		return ret;
	}

	template <typename V>
	static constexpr size_t keyOf()
	{
		return VariantImpl<Values...>::template keyOf<typename std::remove_reference<V>::type>();
	}

	template <typename V>
	const V& view() const
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		return *reinterpret_cast<const V*>(data);
	}

	template <typename V>
	V& get()
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		return *reinterpret_cast<V*>(data);
	}

	template <typename V>
	void set(const V& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		if (key() != -1) VariantImpl<Values...>::destroy(_key, data);
		_key = keyOf<V>();
		*(reinterpret_cast<V*>(data)) = v;
	}

	template <typename V>
	void set(V&& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		if (key() != -1) VariantImpl<Values...>::destroy(_key, data);
		_key = keyOf<V>();
		*(reinterpret_cast<typename std::remove_reference<V>::type*>(data)) = v;
	}

	size_t key() const
	{
		return _key;
	}

private:
	char data[VariantImpl<Values...>::sizeOf()];
	size_t _key;
};

} // namespace tuple_utils
