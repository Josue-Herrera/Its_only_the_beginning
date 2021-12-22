#include <algorithm>
#include <numeric>
#include <memory>
#include <optional>
#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>

#include <Eigen/Dense>

#include "..\include\tester.h"
//#include "..\cc.h"
#include "..\tester.h"
#include "..\iterators.h"
#include "..\kdtree.h"
#include "..\zmq_process.h"
#include "..\DataSets.h"
namespace clustering {
	namespace optics {
		
		template<class T>
		struct optics {

		};
		
	}
}

using test_t = typename double;

struct test {
	test_t field;
	double member_fun(double, double);
};

template <typename, typename = void>
struct has_field : std::false_type {};

template <typename T>
struct has_field< T, std::void_t< // here is where you would add functional requirements  super strict, your struct must have a certain value;
	decltype(T::field),
	decltype(&T::member_fun)
	>> 
: std::true_type{};

template<typename T>
constexpr bool has_field_v = has_field<T>::value;

template<typename T>
using requires_field = typename std::enable_if_t<has_field_v<T>, T>;

template <class T, typename requires_field <T> * = nullptr >
auto fun(T & value) {
	return value.field * value.field;
}


void old() {


	using std::begin;
	using std::end;
	using iterator_algo::size_of;
	using data_set::cities::operator==;

	auto num_cites = size_of(data_set::cities::california);
	std::vector cities(data_set::cities::california, data_set::cities::california + num_cites);
	std::vector cities2 = cities;
	
	kdtree<data_set::cities::city, 2> tree(cities);
	auto search_val = tree.search(cities[30]);

	std::vector< data_set::cities::city> empty;
	kdtree<data_set::cities::city, 2> tree2(empty);

	auto value2 = tree2.insert(cities[0]);
	auto s = tree2.insert(cities[30]);

	s->data_;
}


//#include "../json_addon.h"

auto main() -> int
{
	//double start = 0, end = 10, percent_dist_from_start_to_end = .5;
	//double test = std::lerp(start, end, percent_dist_from_start_to_end);
	//Eigen::Vector3d test2{ 1,1,1 };
	//Eigen::Vector3d s{ Eigen::Vector3d::Constant(1) };
	//std::cout << test2 + (s * test) ;

	//json_test();
	practice::test_case();
	return 0;

}