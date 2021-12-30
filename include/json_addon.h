
#include <vector>
#include <iostream>
#include <../prettyprint.hpp>
//#include <../boost_1_75_0/boost/json.hpp>
#include <../boost_1_75_0/boost/pfr.hpp>
#include <../boost_1_75_0/boost/type_index.hpp>
#include <complex>

//namespace json = boost::json;

struct pfr_demo {
	int f;
};

struct pfr_tester {
	std::vector<std::complex<float>> tbl;
	pfr_demo demo;
};

template<class T>
const constexpr auto is_container_v = pretty_print::is_container<T>::value;

template<class T>
using enable_if_container = typename std::enable_if_t<is_container_v<std::remove_cv_t<T>>, void>;

template<class T>
using enable_if_flat_struct = typename std::enable_if_t<std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>, void>;

template<class T, enable_if_container<T> * = nullptr>
void print_all(T const & v) {
	std::cout << "container : " << v << "\n";
}

template<class T, enable_if_flat_struct<T>* = nullptr>
void print_all(T const& val) {
	std::cout << "struct :  \n";

}

void json_test() {

	//json::value val{ {"Test", {1,2,3} } };
	
	pfr_tester temp{{{0.1f,1.f},{0.1f,1.f},{0.1f,1.f},{0.1f,1.f}}  , { {1} }};

	boost::pfr::for_each_field(temp, [] (auto && v, size_t idx) {

		//std::cout << v <<"\n";
		print_all(v);
	});
}