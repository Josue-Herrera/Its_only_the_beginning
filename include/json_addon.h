
#include <vector>
#include <iostream>
#include <../prettyprint.hpp>
//#include <../boost_1_75_0/boost/json.hpp>
#include <../boost_1_75_0/boost/pfr.hpp>
#include <../boost_1_75_0/boost/type_index.hpp>
#include <complex>

//namespace json = boost::json;

struct testing_shit {
	double so;
};

struct pfr_demo {
	int f;
	testing_shit fs;
};


struct pfr_demo2 {
	pfr_demo f;
};

struct pfr_tester {
	std::vector<std::complex<float>> tbl;
	pfr_demo2 demo;
};

template<class T>
using remove_cvref_t = typename std::remove_cv_t<std::remove_reference_t<T>>;

template<class T>
static const constexpr auto is_container_v = pretty_print::is_container<remove_cvref_t<T>>::value;

template<class T>
static const constexpr auto  is_flat_struct_v = std::is_standard_layout_v<remove_cvref_t<T>> && std::is_trivially_copyable_v <remove_cvref_t<T>>;


template<class T>
void print_all(T & v) {
	if constexpr (is_container_v<decltype(v)>)
		std::cout << "container : " << v << "\n";
	else if constexpr (is_flat_struct_v <decltype(v)>)
		std::cout << "struct :  \n";
	else if constexpr (std::is_aggregate_v <remove_cvref_t<decltype(v)>>) {
		std::cout << "its aggro :  \n";
		boost::pfr::for_each_field(v, [](auto & inner) {
			print_all(inner);
		});
	}
}

void json_test() {

	//json::value val{ {"Test", {1,2,3} } };
	
	pfr_tester temp{{{0.1f,1.f},{0.1f,1.f},{0.1f,1.f},{0.1f,1.f}}  , { {1} }};
	std::cout << " is_flat_struct_v : " << is_flat_struct_v<pfr_tester> << "\n";
	std::cout << " is_aggregate_v : " << std::is_aggregate_v<pfr_tester> << "\n";
	
	print_all(temp);
}