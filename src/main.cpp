#include <algorithm>
#include <numeric>
#include <memory>
#include <optional>
#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>

#include <Eigen/Dense>


#include "..\ca_cities_location.h"
#include "..\iterators.h"
#include "..\kdtree.h"
#include "..\devs.h"
#include "..\zmq_process.h"



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


#include "../json_addon.h"
#include <string_view>
#include <limits>


// *** Task Stealing System ***
#include <atomic>
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

// *** Common type vocabulary *** //
using function_signiture_t = void();
using function_capture_t   = std::function<function_signiture_t>;

class notification_queue {
	// *** notification_queue type vocabulary *** //
	using queue_t              = std::deque<function_capture_t>;
	using mutex_t              = std::mutex;
	using lock_t			   = std::unique_lock<mutex_t>;
	using cond_var_t           = std::condition_variable;
	
	cond_var_t      ready;
	queue_t         queue;
	mutex_t         mutex;
	bool            finished{ false };

public: 

	//
	// Attempt to pop something but if the qu is empty or if its busy it will return false.
	// 
	// 

	bool try_pop(function_capture_t& func) {
		lock_t lock{ mutex, std::try_to_lock };
		if (!lock || queue.empty()) return false;
		func = std::move(queue.front());
		queue.pop_front();
		return true;
	}

	// try_push 
	// 
	//
	template<class Function>
	bool try_push(Function && func) {
		{ 
			lock_t lock{ mutex, std::try_to_lock };
			if (!lock) return false;
			queue.emplace_back(std::forward<Function>(func));
		}
		ready.notify_one();
		return true;

	}

	void done() {
		{
			lock_t lock{ mutex };
			finished = true;
		}
		ready.notify_all();
	}

	bool pop(function_capture_t& func) {
		lock_t lock { mutex };
		while (queue.empty() && !finished) ready.wait(lock);
		if (queue.empty()) return false;
		func = std::move(queue.front());
		queue.pop_front();
		return true;
	}

	template<class Function>
	void push(Function&& func) {
		lock_t lock{ mutex };
		queue.emplace_back(std::forward<Function>(func));
	}

};


class task_system {
	// *** task_system type vocabulary *** //
	using thread_container_t = std::vector<std::thread>;
	using notifications_t    = std::vector<notification_queue>;
	using atomic_index_t     = std::atomic<unsigned>;

	const unsigned			count    { std::thread::hardware_concurrency() };
	thread_container_t		threads;
	notifications_t		    notifications;
	atomic_index_t			index{ 0 };

	void run(unsigned i) {
		while (true) {
			function_capture_t func;
			for (unsigned n = 0; n != count; ++n) {
				if ( notifications[(i + n) & count].try_pop(func) ) 
					break;
			}

			if (!func && !notifications[i].pop(func))
				break;
			func();
		}
	}

public:
	task_system() {
		for (unsigned n = 0; n != count; ++n) {
			threads.emplace_back([&, n] { run(n); });
		}
	}

	~task_system() {
		for (auto& e : notifications) e.done();
		for (auto& e : threads) e.join();
	}
	
	constexpr static const int k_bound = 48;

	template<class Function>
	void async_(Function&& work) {
		auto i = index++;
		for (unsigned n = 0; != count * k_bound; ++n) {
			if(notifications[(i + n) % count].try_push(std::forward<Function>(work)) 
				return;
		}
		notifications[i % count].try_push(std::forward<Function>(work));
	}


};



auto main() -> int
{
	//using namespace std::literals::string_view_literals;

	//double start = 0, end = 10, percent_dist_from_start_to_end = .5;
	//double test = std::lerp(start, end, percent_dist_from_start_to_end);
	//Eigen::Vector3d test2{ 1,1,1 };
	//Eigen::Vector3d s{ Eigen::Vector3d::Constant(1) };
	//std::cout << test2 + (s * test) ;
	//zmqp::publish_test();
	//zmqp::lambda_pack_test();
	//json_test();
	//practice::test_case();
	//dsm::DataSets::create("D:\\workspace\\datasets\\"sv);

	
	size_t k = 4;
	std::vector<int> arr = { 1, 12, -5, -6, 50, 3 };

	auto BAD_findMaxAvg = [](auto& arr, auto k) {
		using nlf = std::numeric_limits<float>;
		auto max = -nlf::infinity();

		for (size_t i = 0; i < arr.size(); ++i) {

			int sum = 0;
			for (size_t j = 0; j < k; ++j) {

				if (!(i + (k - size_t{ 1 }) > arr.size() - size_t{ 1 })) {
					sum += arr[i + j];
				} else {
					i = arr.size();
					j = k;
				}
			}

			if (sum != 0) {
				max = std::max(max, float(sum)/float(k));
			}
		}

		if (max == -nlf::infinity()) {
			return 0.0f;
		}

		return max;
	};

	auto returns = BAD_findMaxAvg(arr, k);
	std::cout << "BAD_findMaxAvg : " << returns << std::endl;

	auto Better_findMaxAvg = [](auto& nums, auto k){
		using nlf  = std::numeric_limits<float>;
		
		auto window_sum   = 0; 
		auto start        = 0;
		auto max          = -nlf::infinity();

		for (size_t window_end {0}; window_end < nums.size(); window_end++) {
			
			window_sum += nums[window_end];
			
			if (window_end - start + size_t{ 1 } == k) {
				max = std::max(max, float(window_sum) / float(k));
				window_sum -= nums[start];
				start++;
			}
			
		}
		
		return max;
	};

	auto Better_findMaxAvg2 = [](auto& nums, auto k) {\

		auto window_sum = 0;
		auto window_start      = std::begin(nums);
		auto max        = -std::numeric_limits<float>::infinity();

		// Main Part if the window 
		for (auto window_end{ std::begin(nums) }; window_end != std::end(nums); window_end++) {
			window_sum += *window_end;
		//

			// *** Condition/Continuatuon IS VARIABLE for each window *** ///
			// Parameterized around : window_end, window_start, max, k.
			if (window_end - window_start + size_t{ 1 } == k)
			{
				max = std::max(max, float(window_sum) / float(k));
				window_sum -= *window_start;
				window_start++;
			}

		}

		return max;
	};

	// Sliding Window Bus Algo :
	// Conditional Predicate Assignment to enter window ? 
	// like a toll troll. before i can compare you must pass a test. 
	//		
	// 
	// 
	// Fixed Length      : max sum subarray of size k
	// Dynnamic Variant  : smallest sum >= to some value S
	// Dyn Var w/ Aux DS : [using a hashmap or something]
	//					   longest substring w/ no more than k distinct character
	//					   - string permutations
	//
	// Key Commonalities!
	// - everything grouped sequentailly
	// 
	// fixate the window around the comm
	// - longest/smallest/contains
	// - max/min 
	// 
	// 
	// 
	// 
	// 
	// Recurrence Relations
	//	Homogenous
	//  Non-hhomogenous
	// 
	// 
	// Amdahl's law : 
	// 
	//






	auto returns2 = Better_findMaxAvg2(arr, k);
	std::cout << "Better_findMaxAvg : " << returns2 << std::endl;

	return 0;


}
