#include <memory>
#include <cstddef>
#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>
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
	double x;
	double sigmas;

	obs (int n, double xs, double sig) :
	num(std::move(n)), x(std::move(xs)), sigmas(std::move(sig)) {
		 std::cout << "I am being constructed.\n";
	}
	obs (obs & other):
	num((other.num)), x((other.x)), sigmas((other.sigmas)) {
		 std::cout << "I am copy constructed.\n";
	}

	obs (obs && other) :
	num(std::move(other.num)), x(std::move(other.x)), sigmas(std::move(other.sigmas)) {
		 std::cout << "I am move constructed.\n";
	}
	
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
index_iterator<T> index (T last) {
	return { last };
}
template< class T>
index_iterator<size_t> index(std::vector<T> & cont) {
	return { cont.size() };
}

template<class T>
range_generator<T> range (T first, T last) {
	return { first , last };
}

auto main() -> int
{
	std::vector a{ 0,1,2,3,4,5,6,7,8,9 };
	tracker tracker_;
	track t1 ;
	obs o1(1,2,3); 
	for(auto i : index(a)) {
	
		t1.num = (int) i;
		o1.num = (int) (10 - i);
		tracker_.track_list_.emplace_back(t1);
		tracker_.obs_list_.emplace_back(o1);
	}

	int counter = 0;
 	auto condition = [](auto&& track, auto&&obs){ return (track.num  > 4 && obs.num  > 6 ) ; };
	auto process   = [&counter](auto&& track, auto&&obs) { counter++; printf("t-%d,o-%d\n", track.num, obs.num); };
	

	{ 
		for (auto & t : tracker_.track_list_ ){
			for (auto & o : tracker_.obs_list_ ){
				if( !condition(t, o))
					continue;
				process(t,o);
				}
		}
	}
	
	
	{
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