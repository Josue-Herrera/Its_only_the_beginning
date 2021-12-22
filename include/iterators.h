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
		: storage{ std::forward<concrete_type>(object) }
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
#include <iostream>
#include <type_traits>
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

	size_t test_overload(size_t d, tag1) {
		std::puts("Tag1 OverLoad\n");
		return d;
	}
	size_t test_overload(size_t d, tag2) {
		std::puts("Tag2 OverLoad\n");
		return d;
	}
	size_t test_overload(size_t d, tag3) {
		std::puts("Tag3 OverLoad\n");
		return d;
	}
	size_t test_overload(size_t d, tag4) {
		std::puts("Tag4 OverLoad\n");
		return d;
	}
	template<class T, class U>
	double wow(T &&t, U && u) {
		return (double)test_overload(std::forward<U>(u), std::forward<T>(t));
	}

	template<class T>
	Eigen::Vector3d create_column_vector_3x1(T) {
		return type_tag_fun(T{});
	}
	template<class... Args>
	auto create_matrix_from_variadic_typelist() {
		using matrix_type = Eigen::Matrix<double, 3 * sizeof...(Args), sizeof...(Args)>;
		matrix_type temp;
		temp << (create_column_vector_3x1(Args{}), ...);
		return temp;
	}
		
#include <iostream>

#include <Eigen/Core>
#include <utility>

	namespace eigen_util {
		using usize = decltype(sizeof(0));

		namespace detail {
			struct Empty {};
			using EmptyArr = Empty[];

			template <typename Seq, typename...>
			struct CoalesceImpl;

			template <usize I, typename T>
			using IgnoreIndex = T;

			template <usize... Is, typename T>
			struct CoalesceImpl<std::index_sequence<Is...>, IgnoreIndex<Is, T>...> {
				using Type = T;
			};

			template <typename T>
			inline constexpr auto add_array_impl_dyn(T const* arr, usize N) noexcept -> T {
				T acc = 0;
				for (usize i = 0; i < N; ++i) {
					if (arr[i] == T(Eigen::Dynamic))
						return arr[i];
			
					acc += arr[i];
				}
				return acc;
			}
			template <typename T, usize N>
			inline constexpr auto add_array_impl(T const (&arr)[N]) noexcept -> usize {
				return detail::add_array_impl_dyn(static_cast<T const*>(arr), N);
			}

			template <typename T>
			inline constexpr auto common_array_impl_dyn(T const* arr, usize N) noexcept -> T {
				T dim = T(Eigen::Dynamic);
				for (usize i = 0; i < N; ++i) {
					if (dim == T(Eigen::Dynamic))
						dim = arr[i];
					else if ((dim != arr[i]) && !(arr[i] == T(Eigen::Dynamic)))
						std::terminate();
				}
				return dim;
			}
			template <typename T, usize N>
			inline constexpr auto common_array_impl(T const (&arr)[N]) noexcept -> usize {
				return detail::common_array_impl_dyn(static_cast<T const*>(arr), N);
			}

		} // namespace detail

		template <usize... Ns>
		struct AddDims
			: std::integral_constant < usize, detail::add_array_impl({ Ns... }) > {};

		template <usize... Ns>
		struct CommonDim
			: std::integral_constant < usize, detail::common_array_impl({ Ns... }) > {};

		template <typename... Ts>
		using Coalesce = typename detail::
			CoalesceImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...>::Type;

		template <
			typename... Mats,
			typename ConcatMat = Eigen::Matrix<                          //
			Coalesce<typename Mats::Scalar...>,                      //
			int(AddDims<usize(Mats::RowsAtCompileTime)...>::value),  //
			int(CommonDim<usize(Mats::ColsAtCompileTime)...>::value) //
			>>
			auto append_vectors_together(Mats const&... mats) -> ConcatMat {
			ConcatMat concat{
					Eigen::Index(detail::add_array_impl({usize(mats.rows())...})),
					Eigen::Index(detail::common_array_impl({usize(mats.cols())...})),
			};

			Eigen::Index idx = 0;

			(void)detail::EmptyArr{
					(void(concat.middleRows(idx, mats.rows()) = mats),
					void(idx += mats.rows()),
			detail::Empty{})...,};

			return concat;
		}
	} // namespace eigen_util


	template <template<class...> class Tag_list, class... Args>
	void test_tag(Tag_list<Args...>&& test) {

		auto hx = [](auto&& test) {
			std::array<double, sizeof...(Args)> d = { wow(Args{}, sizeof...(Args))... };
			for (auto e : d) {
				std::cout << e << std::endl;
			}
			return d;
		}(test);
	
	}

		//sum = test_overload(test.value,Args) + ;

	void test_case() {
		using tags = typename tag_list<tag1, tag2>;
	
		test_tag(tags{});

		Eigen::Vector3d a;
		Eigen::Vector4d b;
		Eigen::MatrixXd c(6, 1);

		a.setRandom();
		b.setRandom();
		c.setRandom();

		auto concat = eigen_util::append_vectors_together(a, b, c);
		std::cout << a.transpose() << '\n';
		std::cout << b.transpose() << '\n';
		std::cout << c.transpose() << '\n';

		std::cout << concat.transpose() << '\n';
	}
}