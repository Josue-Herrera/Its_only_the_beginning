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

#include <../boost_1_75_0/boost/pfr.hpp>

namespace zmqp {
  
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
    index_iterator<T> enumarate(T last) {
        return { last };
    }

    using endpoints_t = std::vector<std::string>;
    using sockets_t   = std::vector<zmq::socket_t>;

    struct setup_t {
        endpoints_t pubs;
        endpoints_t subs;
        endpoints_t reqs;
        endpoints_t reps;
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

    struct header_t {
        int id;
    };

    template<class body_t>
    struct zmessage_t {
        header_t header;
        body_t body;
    };

    template<typename T, typename _ = void>
    struct is_container : std::false_type {};

    template<typename... Ts>
    struct is_container_helper {};

    template<typename T>
    struct is_container<
        T,
        std::conditional_t<
        false,
        is_container_helper<
        typename T::value_type,
        typename T::size_type,
        typename T::allocator_type,
        typename T::iterator,
        typename T::const_iterator,
        decltype(std::declval<T>().size()),
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        decltype(std::declval<T>().cbegin()),
        decltype(std::declval<T>().cend())
        >,
        void
        >
    > : public std::true_type{};

    template<class T>
    constexpr bool is_container_v = is_container<T>::value;

    class zprocess_t {

        using st = zmq::socket_type;
        using io_events_t = std::vector<zmq::poller_event<>>;
        
        sockets_t pubs;
        sockets_t subs;
        sockets_t reqs;
        sockets_t reps;
        zmq::context_t  context;
        zmq::poller_t<> poller;
        io_events_t input_events;
        zmq::multipart_t messages;
        std::size_t poll_keys_count;
        
        void setup(setup_t const& e) noexcept {
            bind<st::pub>(pubs, e.pubs, context, poller);
            bind<st::rep>(reps, e.reps, context, poller);
            connect<st::sub>(subs, e.subs, context, poller);
            connect<st::req>(reqs, e.reqs, context, poller);
            input_events.resize(poller.size());
            poll_keys_count = input_events.size();
        }

        zprocess_t(setup_t const& values) :
            context{ values.context_n }, poll_keys_count{ 0 }, messages{} {
            setup(values);
        }
        
        auto poll(std::chrono::milliseconds timeout) { return poller.wait_all(input_events, timeout); } 
        auto poll_keys()                             { return enumarate(poll_keys_count); }
        

        template<class T>
        void to_zmq(zmq::multipart_t & messages, T const& value) {
            if constexpr (std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>) {
                messages.addtyp(value);
            }
            else if constexpr (is_container_v<T>) {
                messages.add(zmq::message_t{ value.begin(), value.end() });
            } 
        }

        template<class T>
        void from_zmq(zmq::multipart_t& messages, T & value) {
            if constexpr (std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>) {
                value = messages.poptyp(value);
            }
            else if constexpr (is_container_v<T>) {
                using value_t = T::value_type;
                auto msg = messages.pop();
                std::span data { msg.data<value_t>(), msg.size() / sizeof(value_t) };
                value.reserve(data.size());
                std::copy(data.begin(), data.end(), std::back_inserter(value));
            }
        }

        auto peek_header() {
            return messages.peektyp<header_t>(std::size_t{ 0 });
        }
       
        auto get_header() {
            return messages.poptyp<header_t>();
        }

        template<class T, class To_ZMQ_Func>
        auto to_buffer(T const & data, To_ZMQ_Func to_zmq) {
            auto&& data_to_send = to_zmq(data);
            boost::pfr::for_each_field(data_to_send, [&](auto const & field) {
                to_zmq(messages, field);
            });
        }

        template<class T>
        void from_buffer(T & data) {
            boost::pfr::for_each_field(data, [&](auto& field) {
                from_zmq(messages, field);
            });
        }
    };
  

}