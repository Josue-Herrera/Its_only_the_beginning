#pragma once 

/* zmq.hpp includes
#include <cassert>
#include <cstring>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
ZMQ_CPP11
#include <array>
#include <chrono>
#include <tuple>
#include <memory>
#include <type_traits>
ZMQ_CPP17
#include <string_view>
#include <optional>
*/
#define ZMQ_BUILD_DRAFT_API 
/*zmq_addon.hpp Includes
#include "zmq.hpp"
#include <deque>
#include <iomanip>
#include <sstream>
#include <stdexcept>
ZMQ_CPP11
#include <limits>
#include <functional>
#include <unordered_map>
*/
#include <thread>
#include <zmq_addon.hpp>
#include <iostream>
#include <span>
#include <charconv>



struct simple {
	int test;
	int t2;
	double fg;
	int sd;
};

struct z1 { // must be fixed size from front whatever that means
	simple m1;
	std::vector<std::complex<float>> m2;
};

template<typename T>
using val_t = typename T::value_type;

template<typename T >
void print(T const& cont) {
    std::cout << " { ";
    for (auto& elem : cont) {
        std::cout << elem << ", ";
    }
    std::cout << " }\n";
}
#include<chrono>
namespace chrono = std::chrono;
struct timer {
    using clock = chrono::system_clock;
    using milliseconds = chrono::milliseconds;

    int num;
    clock::time_point start;

    timer(int n) :
        num{ n },
        start{ clock::now() }
    {};
   
    ~timer() {
       const auto duration = chrono::duration_cast<milliseconds>(clock::now()-start).count();
       std::cout << "duration : " << duration << " millieseconds " << " iterations : " << num;
    }
};
#include "..\iterators.h"
using ST = typename zmq::socket_type;
void simple_pub() {

	zmq::context_t ctx{ 1 };
	const std::string addr{ "tcp://127.0.0.1:5555" };

	zmq::socket_t pub{ ctx, ST::req };
	pub.connect(addr);

	zmq::multipart_t mp_msg{};
	z1 zmq_data{ {9,9, 2.4,9} };
	std::complex<float> s{ 1,1 };
	zmq_data.m2.assign(50'000, s);

	std::cout << "zmq data { " << zmq_data.m1.fg << " }\n";
    int iter = 1'000;
    {
        timer sn(iter);
        using namespace iterator_algo;
        for (auto & i : index(iter)) {
            zmq::multipart_t recv_msg{};
            mp_msg.addtyp(zmq_data.m1);
            mp_msg.add(zmq::message_t{ zmq_data.m2 });
            mp_msg.send(pub);
            std::cout << "zmq data { " << zmq_data.m1.fg << " " << i <<" }\n";
            std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });
            recv_msg.recv(pub);
        }
    }

	ctx.shutdown();
	ctx.close();
}

void simple_sub() {

	zmq::context_t ctx{ 1 };
	const std::string addr = "tcp://127.0.0.1:5555";

	zmq::socket_t sub{ ctx, ST::sub };
	sub.connect(addr);

    std::string str{ "x" };
    const int value = 9;
    std::to_chars(str.data(), str.data() + str.size(), value);
    sub.set(zmq::sockopt::subscribe, str);

	std::vector<std::complex<float>> v_t;

    int iter = 10;
    {
       while(true) {
            zmq::multipart_t mp_msg{};
            zmq::multipart_t send_msg{};
            zmq::multipart_t uncompressed{};
            mp_msg.recv(sub);
            //uncompressed = mp_msg.decode(mp_msg.pop());
            try {
                using namespace std;
                int count{ 0 };
                auto b2 = mp_msg.poptyp<simple>();
                cout << "zmq data { " << b2.test << ", "<< b2.sd << " }\n";
                //send_msg.addtyp(b2);
               // auto msg = uncompressed.pop();
               //auto cpy_span = [&](std::span<std::complex<float>> const& data) { std::copy(data.begin(), data.end(), std::back_inserter(v_t)); };
                //cpy_span({ msg.data<std::complex<float>>(), msg.size() / sizeof(std::complex<float>) });
                //print(v_t);
                //send_msg.send(sub);
            }
            catch (std::exception e) { std::cout << e.what() << std::endl; }
        }
        
    }
	ctx.shutdown();
	ctx.close();
}


namespace zmqp {
    template<class T, class = void>
    struct has_size : std::false_type {};
 
    template<class T>
    struct has_size <T, 
        std::void_t< decltype(std::declval<std::size_t&>() = std::declval<T const&>().size())>>
        : std::true_type { };

    template<class T>
    constexpr bool has_size_v = has_size<T>::value;

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
    template<typename T, typename std::enable_if_t<has_size_v<T>,T>>
    index_iterator<size_t> index(T & cont) {
        return { cont.size() };
    }

    template<class T = int>
    using filters_t   = std::vector<std::vector<T>>;
    using endpoints_t = std::vector<std::string>;
    using sockets_t   = std::vector<zmq::socket_t>;

    struct setup_t {
        endpoints_t pubs;
        endpoints_t subs;
        endpoints_t reqs;
        endpoints_t reps;
        filters_t<>  filters;
        int context_n;
    };

    template<zmq::socket_type st, class poller_t>
    auto bind(sockets_t& sockets, endpoints_t const& endpoints, zmq::context_t & ctx, poller_t& poller) noexcept {
        for (auto const& endpoint : endpoints) {
            auto& socket_ref = sockets.emplace_back(ctx, st);
            socket_ref.bind(endpoint);
            if constexpr (st != zmq::socket_type::pub)
                poller.add(socket_ref, zmq::event_flags::pollin);
        }
    }

    template<zmq::socket_type st, class poller_t>
    auto connect(sockets_t& sockets, endpoints_t const& endpoints, zmq::context_t& ctx, poller_t& poller) noexcept {
        for (auto const& endpoint : endpoints) {
            auto& socket_ref = sockets.emplace_back(ctx, st);
            socket_ref.connect(endpoint);
            poller.add(socket_ref, zmq::event_flags::pollin);
        }
    }

    class zprocess_t {
        using key_t = int;
        using st = zmq::socket_type;
        using io_events_t = std::vector<zmq::poller_event<key_t>>;
        
        sockets_t pubs;
        sockets_t subs;
        sockets_t reqs;
        sockets_t reps;
        zmq::context_t  context;
        zmq::poller_t<key_t> poller;
        io_events_t input_events;
        zmq::multipart_t messages;
        std::size_t poll_hits;
        
        void setup(setup_t const& e) noexcept {
            bind<st::pub>(pubs, e.pubs, context, poller);
            bind<st::rep>(reps, e.reps, context, poller);
            connect<st::sub>(subs, e.subs, context, poller);
            connect<st::req>(reqs, e.reqs, context, poller);
            input_events.resize(poller.size());
        }

        zprocess_t(setup_t const& values) :
            context{ values.context_n }, poll_hits{ 0 }, messages{} {
            setup(values);
        }
        
        template<class T>
        auto to_buffer(T & data) {
            
        }

        template<class T>
        auto from_buffer() {

        }
    };
  

}