#pragma once 

#include <utility>
#include <variant>
#include <iostream>
#include <any>

#define FWD(...) \
std::forward<decltype<__VA_ARGS__)>(__VA_ARGS__)

#define LIFT(X) [](auto && ... args)    \
	noexcept(noexcept(X(FWD(args)...))) \
	      -> decltype(X(FWD(args)...))  \
	         { return X(FWD(args)...); }

template<typename interface>
class implementation
{
public:
	template<typename concrete_type>
	implementation(concrete_type&& object)
		: storage{ std::forward<concrete_type >(object) }
		, getter{ [] (std::any& storage)->interface&
					{
						return std::any_cast<concrete_type&>(storage);
					}
	}
	{}

	interface* operator-> () { return &getter(storage); }

private:
	std::any storage;
	interface& (*getter)(std::any&);
};

template <typename t>
using stub = implementation<t>;

namespace iterator_algo {
	template<class Container, class Predicate>
	void remove_elements_if(Container& container, Predicate predicate) {
		container.erase(
			std::remove_if(std::begin(container), std::end(container), predicate),
			std::end(container)
		);
	}

	template<class Container, class Predicate>
	void sort(Container& container, Predicate predicate) {
		std::sort(std::begin(container), std::end(container), predicate);
	}

	template <typename T>
	struct range_generator
	{
		T first;
		T last;

		struct iterator {
			T value;

			using iterator_category = std::input_iterator_tag;
			using value_type = T;

			iterator& operator++() { ++value; return *this; }
			iterator operator++(int) = delete;

			bool operator==(iterator const& other) const { return value == other.value; }
			bool operator!=(iterator const& other) const { return !(*this == other); }

			T const& operator*()  const { return value; }
			T const* operator->() const { return std::addressof(value); }
		};

		iterator begin() { return{ first }; }
		iterator end() { return{ last }; }

	};

	template <typename T>
	struct index_iterator
	{
		T last;
		struct iterator {
			T value;

			using iterator_category = std::input_iterator_tag;
			using value_type = T;

			iterator& operator++() { ++value; return *this; }
			iterator operator++(int) = delete;

			bool operator==(iterator const& other) const { return value == other.value; }
			bool operator!=(iterator const& other) const { return !(*this == other); }

			T const& operator*()  const { return value; }
			T const* operator->() const { return std::addressof(value); }
		};

		iterator begin() { return{ 0 }; }
		iterator end() { return{ last }; }

	};

	template<std::integral T >
	index_iterator<T> index(T last) {
		return { last };
	}
	template< class T>
	index_iterator<size_t> index(std::vector<T>& cont) {
		return { cont.size() };
	}





	template <class T>
	[[nodiscard]] constexpr auto size_of(T const& arr){
		return sizeof(arr) / sizeof(arr[0]);
	}
	


	template<class T>
	range_generator<T> range(T first, T last) {
		return { first , last };
	}
}