#include <memory>
#include <cstddef>
#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>
#include <coroutine>
#include <tuple>
#include <variant>
#include <optional>
#include <typeinfo>

template <class T>
struct Mallocator
{
  typedef T value_type;
 
  Mallocator () = default;
  template <class U> constexpr Mallocator (const Mallocator <U>&) noexcept {}
 
   //[[nodiscard]]
   T* allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      throw std::bad_array_new_length();
 
    if (auto p = static_cast<T*>(std::malloc(n*sizeof(T)))) {
      report(p, n);
      return p;
    }
 
    throw std::bad_alloc();
  }
 
  void deallocate(T* p, std::size_t n) noexcept {
    report(p, n, false);
    std::free(p);
  }
 
private:
  void report(T* p, std::size_t n, bool alloc = true) const {
    std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T)*n
	  << " Type : " <<  typeid(T).name() << " size : " << sizeof(T) << " n times : " << n
      << " bytes at " << std::hex << std::showbase
      << reinterpret_cast<void*>(p) << std::dec << '\n';
  }
};
template <class T, class U>
bool operator==(const Mallocator <T>&, const Mallocator <U>&) { return true; }
template <class T, class U>
bool operator!=(const Mallocator <T>&, const Mallocator <U>&) { return false; }

struct obs {
	int num;
	double x[3];
	double sigmas[3];
};

struct track {
	int num;
	double x[6];
	double p[6][6];
};

struct tracker {
	using Track_List = std::vector< track, Mallocator<track> >;
	using Obs_List 	 = std::vector< obs  , Mallocator<obs>   >;
	
	tracker() { 
		track_list_.reserve(10000);
		obs_list_.reserve(10000);
	}
	~tracker(){};

	Obs_List obs_list_;
	Track_List track_list_;
};

template<class Container, class Predicate>
void remove_elements_if(Container & container, Predicate predicate) {
	container.erase(
		std::remove_if(std::begin(container), std::end(container), predicate ),
		std::end(container)
	);
}

template<class Container, class Predicate>
void sort(Container & container, Predicate predicate) {
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

		iterator& operator++(){	++value; return *this;}
		iterator operator++(int) = delete;

		bool operator==(iterator const& other) const { return value == other.value; }
		bool operator!=(iterator const& other) const { return !(*this == other); }

		T const& operator*()  const { return value; }
		T const* operator->() const { return std::addressof(value); }
	};

	iterator begin() { return{ first }; }
	iterator end()   { return{ last  }; }
	

template <typename T>
range_generator<T> range(T first, T last) { return { first, last }; }

template<class T>
struct generator {
	struct promise_type {
		std::variant<T const *, std::optional<std::exception_ptr>> value;
	};
	using handle_type = std::coroutine_handle<promise_type>;
};


#include <chrono>

struct timer {
	timer() { start = std::chrono::high_resolution_clock::now(); };
	~timer() 
	{
		auto stop = std::chrono::high_resolution_clock::now(); 
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
		std::cout << "Duration " << duration.count() << " microseconds\n";
	}

	std::chrono::_V2::system_clock::time_point start;
	
};

auto main() -> int
{
	using namespace std::chrono;

	tracker tracker_;
	track t1 ;
	obs o1; 
	for( int i : range(0, 10)) {
	
		t1.num = i;
		o1.num = 10 - i;
		tracker_.track_list_.emplace_back(t1);
		tracker_.obs_list_.emplace_back(o1);
	}

	int counter = 0;
 	auto condition = [](auto&& track, auto&&obs){ return (track.num  > 4 && obs.num  > 6 ) ; };
	auto process   = [&counter](auto&& track, auto&&obs) { counter++; printf("t-%d,o-%d\n", track.num, obs.num); };
	

	{ timer t;
		for (auto & t : tracker_.track_list_ ){
			for (auto & o : tracker_.obs_list_ ){
				if( !condition(t, o))
					continue;
				process(t,o);
				}
		}
	}
	
	
	{ timer t;
		for (auto & t : tracker_.track_list_ ){
			for (auto & o : tracker_.obs_list_ ){
				if( !(t.num  > 4 && o.num  > 6 ))
					continue;
				counter++; printf("t-%d,o-%d\n", t.num, o.num);
			}
		}
	}




	printf("count : %d\n", counter);

	auto threshold = [](track & item ){ return item.num > 4 ; };
	remove_elements_if(tracker_.track_list_, threshold);

	
	for ( auto & track : tracker_.track_list_) 
		printf("%d\n" , track.num);
	
	
	return 0;
}