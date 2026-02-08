#pragma once

#include <utility>
#include <cstddef>
#include <tuple>
#include <cstring>

namespace tuple_utils
{

template <typename T>
static constexpr T max(T a, T b)
{
	return a > b ? a : b;
}

template <size_t N, typename Value, typename ...Rest>
struct VariantImpl
{
	using Next = VariantImpl<N, Rest...>;

	static constexpr size_t alignOf()
	{
		return max(alignof(Value), Next::alignOf());
	}

	template <typename V>
	static constexpr size_t keyOf()
	{
		return std::is_same<V, Value>::value ? N - sizeof...(Rest) : Next::template keyOf<V>();
	}

	static constexpr size_t sizeOf()
	{
		return max(sizeof(Value), Next::sizeOf());
	}

	static void destroy(const size_t& i, void* p)
	{
		if (i == keyOf<Value>())
			reinterpret_cast<Value*>(p)->~Value();
		else
			Next::destroy(i, p);
	}

	template <typename T, typename ...Params>
	static void switchf(const size_t& key, Params&&... params)
	{
		if (keyOf<Value>() == key)
			T::template callback<Value>(std::forward<Params>(params)...);
		else
			Next::template switchf<T>(key, std::forward<Params>(params)...);
	}

	template <typename F, typename T>
	static void switchf2(const size_t& key, F f, T& t)
	{
		if (keyOf<Value>() == key)
			f(t.template get<Value>());
		else
			Next::template switchf2(key, f, t);
	}
};


template <size_t N, typename Value>
struct VariantImpl <N, Value>
{
	static constexpr size_t alignOf()
	{
		return alignof(Value);
	}

	template <typename V>
	static constexpr size_t keyOf()
	{
		return std::is_same<V, Value>::value ? N : -1;
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
	static void switchf2(const size_t& key, F f, T& t)
	{
		if (keyOf<Value>() == key)
			f(t.template get<Value>());
	}
};


template <typename ...Values>
struct Variant
{
	using tuple = std::tuple<Values...>;
	using Impl = VariantImpl<sizeof...(Values) - 1, Values...>;

	static constexpr size_t size()
	{
		return sizeof...(Values);
	}

	Variant()
	{
		_key = -1;
	}

	Variant(Variant&& v)
	{
		v.switchf2([&] <typename T> (T& value) {
			new (data) T(std::move(value));
		});
		_key = v._key;
	}

	Variant(const Variant& v)
	{
		_key = v._key;
		switchf2([&] <typename T> (T& value) {
			value = v.template view<T>();
		});
	}

	template <typename V>
	explicit Variant(const V& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		_key = keyOf<V>();
		new(data) V(v);
	}

	template <typename V>
	explicit Variant(V&& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		_key = keyOf<V>();
		new (data) typename std::remove_reference<V>::type(v);
	}

	template <typename V>
	Variant& operator = (const V& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		Impl::destroy(_key, data);
		_key = keyOf<V>();
		new(data) V(v);
		return *this;
	}

	Variant& operator = (const Variant& v)
	{
		Impl::destroy(_key, data);
		_key = v._key;
		switchf2([&] <typename T> (T& value) {
			new(data) T(v.template view<T>());
		});
		return *this;
	}

	~Variant()
	{
		Impl::destroy(_key, data);
	}

	template <typename F>
	bool switchf2(F f)
	{
		Impl::template switchf2(_key, f, *this);
		return _key != -1;
	}

	template <typename T, typename ...Params>
	void switchf(Params&&... params)
	{
		Impl::template switchf<T>(_key, std::forward<Params>(params)...);
	}

	template <typename V>
	static Variant construct()
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		Variant ret;
		ret._key = keyOf<V>();
		new (reinterpret_cast<V*>(ret.data)) V();
		return ret;
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

	static Variant fromKey(const size_t& i)
	{
		Variant ret;
		ret._key = i;
		ret.switchf2([] <typename T> (T& t) {
			new (&t) T();
		});
		return ret;
	}

	template <typename V>
	static constexpr size_t keyOf()
	{
		return Impl::template keyOf<typename std::remove_reference<V>::type>();
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
		if (key() != -1) Impl::destroy(_key, data);
		_key = keyOf<V>();
		*(reinterpret_cast<V*>(data)) = v;
	}

	template <typename V>
	void set(V&& v)
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		if (key() != -1) Impl::destroy(_key, data);
		_key = keyOf<V>();
		*(reinterpret_cast<typename std::remove_reference<V>::type*>(data)) = v;
	}

	size_t key() const
	{
		return _key;
	}

	template <typename V>
	size_t keyOf2() const
	{
		static_assert(keyOf<V>() != -1, "Type is not a member of variant.");
		return Variant::keyOf<V>();
	}

private:
	size_t _key;
	alignas(VariantImpl<sizeof...(Values), Values...>::alignOf())
	unsigned char data[Impl::sizeOf()];
};

template <typename ...Values>
struct variant_from_tuple;

template <typename ...Values>
struct variant_from_tuple<std::tuple<Values...>>
{
	using type = Variant<Values...>;
};

} // namespace tuple_utils
