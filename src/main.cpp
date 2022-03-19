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


#include<chrono>
namespace chrono = std::chrono;

template<bool add_new_line>
struct timer {
	using clock = chrono::system_clock;
	using milliseconds = chrono::milliseconds;

	char const* const name;
	clock::time_point start;

	timer(char const* const n) :
		name{ n },
		start{ clock::now() }
	{};

	~timer() {
		const auto duration = chrono::duration_cast<milliseconds>(clock::now() - start).count();
		if constexpr (add_new_line)
			printf("[duration] %lldms [name] %s\n",duration, name);
		else 
			printf("[duration] %lldms [name] %s", duration, name);
		
	}
};

using timer_nl = timer<true>;
using timer_no = timer<false>;

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


	// Attempt to pop something but if the qu is empty or if its busy it will return false.
	bool try_pop(function_capture_t& func) {
		lock_t lock{ mutex, std::try_to_lock };
		if (!lock || queue.empty()) return false;
		func = std::move(queue.front());
		queue.pop_front();
		return true;
	}

	// try_push 
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
	notifications_t		    notifications{count};
	atomic_index_t			index{ 0 };
	unsigned                k_bound {48};

	void run(unsigned i) {
		std::string str{ std::to_string(i) + " worker thread" };
		std::string str2{};
		str2.reserve(1024);
		unsigned task_count{ 0 };
		
		{timer_no t(str.c_str());
		while (true) {

			function_capture_t func;

			for (unsigned n = 0; n != count; ++n) {
				if (notifications[(i + n) % count].try_pop(func)) break;
			}

			if (!func && !notifications[i].pop(func)) break;

			printf("[%s]  ", str.c_str());

			++task_count;
			func();


			str2.clear();
		}
		} /* sliver of time between t desctructor is calls and the print gets called */
		printf(" [task count] : %u\n", task_count);
	}

public:

	explicit task_system (unsigned k) : k_bound{ k } {
		for (unsigned n = 0; n != count; ++n) 
			threads.emplace_back([&, n] { run(n); });
	}

	~task_system() {
		for (auto& ns : notifications) ns.done();
		for (auto& ts : threads)       ts.join();
	}
	


	template<class Function>
	void async_(Function&& work) {
		auto i = index++;
		for (unsigned n = 0; n != count * k_bound; ++n) {
			if(notifications[(i + n) % count].try_push(std::forward<Function>(work))) return;
		}
		notifications[i % count].push(std::forward<Function>(work));
	}


};



auto main() -> int
{

	using namespace std::literals::chrono_literals;

	auto total      = 410;
	auto sleep_time = 3ms;
	auto k_bound    = 50u;
	{ timer_nl t("single threaded");
		for (int i = 0; i < total; ++i) {
			printf("[task #%d]\n", i);
			std::this_thread::sleep_for(sleep_time);
		}
	}
	
	{
		task_system ts{ k_bound };
		{ timer_nl t2("async function");
			for (int i = 0; i < total; ++i) {
				ts.async_([=, &ts]{
					printf("[task #%d]\n", i);
					std::this_thread::sleep_for(sleep_time);
				});
				
			}
		}
	}
	
	
	return 0;


}
