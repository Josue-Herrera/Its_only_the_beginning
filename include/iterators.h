#pragma once 

#include <utility>
#include <variant>
#include <iostream>
#include <type_traits>
#include <Eigen/Core>

#define FWD(...) \
std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

#define LIFT(X) [](auto && ... args)    \
	noexcept(noexcept(X(FWD(args)...))) \
	      -> decltype(X(FWD(args)...))  \
	         { return X(FWD(args)...); }

namespace iterator_algo {
	template<class Container, class Predicate>
	void erase_elements_if(Container & container, Predicate predicate) {
		using std::begin, std::end;
		container.erase(
			std::remove_if(begin(container), end(container), predicate),
			std::end(container)
		);
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



namespace practice {
	// values_equal<a, b, T>::value is true if and only if a == b.
		
	struct tag1{};
	struct tag2{};
	struct tag3{};
	struct tag4{};


	template <class... Tags>
	struct tag_list {
		using type = typename tag_list<Tags...>;
		static constexpr size_t size = sizeof...(Tags);
		int value;
	};


	template<class Iter1, class Iter2, class Iter3, class BinaryPredicate>
	inline void linear_assignment_if(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2, Iter3 out, BinaryPredicate predicate) {
		using std::next;
		

		for (; begin1 != end1; begin1 = next(begin1)) {
			*out = { begin1, end2 };
			for (Iter2 cur2 = begin2; cur2 != end2; cur2 = next(cur2)) {
				if (predicate(*begin1, *begin2)) {
					*out = { begin1, begin2 };
					break;
				}
			}
			out = next(out);
		}
	}


	template<class Iter1, class Iter2, class OIter1, class OIter2, class BinaryPredicate>
	inline void linear_assignment_if(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2,
							  OIter1 outPass, OIter2 outFail, BinaryPredicate predicate) {
		using std::next;

		for (; begin1 != end1; begin1 = next(begin1)) {
			for (Iter2 cur2 = begin2; cur2 != end2; cur2 = next(cur2)) {
				if (predicate(*begin1, *begin2)) {
					*outPass = { begin1, begin2 };
					outPass = next(outPass);
				} else {
					*outFail = { begin1 };
					outFail = next(outFail);
				}
			}
		}
	}

	template<class Iter1, class Iter2, class PassAssignment, class FailAssignment, class BinaryPredicate>
	inline void on_linear_assignment_if(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2,
		PassAssignment pass, FailAssignment fail, BinaryPredicate predicate) {
		using std::next;

		for (; begin1 != end1; begin1 = next(begin1)) {
			for (Iter2 cur2 = begin2; cur2 != end2; cur2 = next(cur2)) {
				if (predicate(*begin1, *begin2)) {
					pass(*begin1, *begin2);
				}
				else {
					fail(*begin1);
				}
			}
		}
	}

	template<class... Predicates>
	inline auto chain(Predicates... predicate) {
		return [=](auto const &... value) {
			return (predicate(value...) && ...);
		};
	}


	void test_case() {
		using std::begin, std::end;
		using vec_int         = std::vector<int>;
		using vec_int_iter    = typename vec_int::iterator;
		using assignment_pair = std::tuple <vec_int_iter, vec_int_iter>;
		using ass_container   = std::vector<assignment_pair>;


		vec_int list1 = { 5,6,7,8,0,2,4 };
		vec_int list2 = { 1,2,3,4,9,23,44 };
		ass_container temp(list1.size());

		auto is_even = [](auto const& value) {
			bool test = value % 2 == 0;
			return test;
		};

		auto is_less_than = [](auto const& threshold) {
			return [&](auto const& value) {
				bool test = value < threshold;
				return test;
			};
		};

		auto l2_new_end = std::partition(begin(list2), end(list2), chain(is_even, is_less_than(20)));
		
		linear_assignment_if(begin(list1), end(list1), begin(list2), l2_new_end, begin(temp), std::equal_to());

		auto pass_assignment = [](auto& one, auto& two) {
				
		};

		// This algo should be called on_assignment. passAssignmentFunc & Failed Assigment Func
		std::for_each(begin(temp), end(temp), [&](auto& tuple_iter) {
			if (auto [l1_iter, l2_iter] = tuple_iter; l2_iter != l2_new_end) {
				
			}
			else {

			}
		});


		
		
	}

}