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
#include<chrono>
#include <../boost_1_75_0/boost/pfr.hpp>
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



namespace zmqp {

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
    using remove_cvref_t = typename std::remove_cv_t<std::remove_reference_t<T>>;

    template<class T>
    constexpr bool is_container_v = is_container<remove_cvref_t<T>>::value;

    template<class T>
    static const constexpr auto  is_flat_struct_v = std::is_standard_layout_v<remove_cvref_t<T>> && std::is_trivially_copyable_v <remove_cvref_t<T>>;

    template<class T>
    static const constexpr auto  is_nested_struct_v = std::is_aggregate_v<remove_cvref_t<T>>;

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
            if constexpr (st == zmq::socket_type::sub)
                socket_ref.set(zmq::sockopt::subscribe, "");

        }
    }

    template<class T>
    void to_zmq_buffer(zmq::multipart_t& messages, T const& value) {
        if constexpr (is_flat_struct_v<T>) {
            messages.addtyp(value);
        }
        else if constexpr (is_container_v<T>) {
            messages.add(zmq::message_t{ value.cbegin(), value.cend() });
        }
        else if constexpr (is_nested_struct_v<T>) {
            boost::pfr::for_each_field(value, [&](auto const& field) {
                to_zmq_buffer(messages, field);
            });
        }
    }

    template<class T>
    void from_zmq_buffer(zmq::multipart_t& messages, T & value) {
        if constexpr (is_flat_struct_v<T>) {
          value = messages.poptyp<T>();
        }
        else if constexpr (is_container_v<T>) {
            using value_t = T::value_type;
            auto msg = messages.pop();
            std::span data{ msg.data<value_t>(), msg.size() / sizeof(value_t) };
            value.clear();
            value.reserve(data.size());
            std::copy(data.begin(), data.end(), std::back_inserter(value));
        }
        else if constexpr (is_nested_struct_v<T>) {
            boost::pfr::for_each_field(value, [&](auto& field) {
                from_zmq_buffer(messages, field);
            });
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

    template<class Body_t, class Fill_Header_F>
    auto to_zmq_message(Body_t const& body, Fill_Header_F Func) {
        return zmessage_t{ { Func() }, body };
    }

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

    public:
        zprocess_t(setup_t const& config) :
            context{ 1 },
            messages{} 
        {
            bind<st::pub>(pubs, config.pubs, context, poller);
            bind<st::rep>(reps, config.reps, context, poller);
            connect<st::sub>(subs, config.subs, context, poller);
            connect<st::req>(reqs, config.reqs, context, poller);
            input_events.resize(poller.size());
        }
        
        inline auto poll(std::chrono::milliseconds timeout = std::chrono::milliseconds{ 1 })
             { return poller.wait_all(input_events, timeout); }
        inline auto poll_keys() { return enumarate(input_events.size()); }

       

        inline auto peek_header() {
            return messages.peektyp<header_t>(std::size_t{ 0 });
        }
       
        inline auto get_header() {
            return messages.poptyp<header_t>();
        }

        template<class T>
        inline void to_buffer(T const & data) {
            to_zmq_buffer(messages, data);
        }

        template<class T>
        inline void from_buffer(T & data) {
            from_zmq_buffer(messages, data);
        }

        inline auto publish(std::size_t index = 0) {
            messages.send(pubs[index]);
        }

        inline auto subscribe(std::size_t index = 0) {
            messages.recv(subs[index]);
        }
    };
  
    struct simple {
        double fg;
        int sd;
    };

    struct z1 { // must be fixed size from front whatever that means
        simple m1;
        std::vector<float> m2;
    };

    inline void publish_test() {
       setup_t config{ 
           {"tcp://127.0.0.1:5555"},//pubs
           {},//subs
           {},//reqs
           {} //reps
       };

       zprocess_t proc { config };
       double d_count = 0.0;
       int i_count = 0;
       z1 temp { {d_count,i_count}, { 0.3f, 0.2f, 0.1f } };

       while (true) {
           std::cout << "z1 { " << temp.m1.fg++ << " : " << temp.m1.sd++
                     << " : " << temp.m2.size() << " } \n\n";
           proc.to_buffer(temp);
           proc.publish();
           std::this_thread::sleep_for(std::chrono::milliseconds(100));
       }
   }

   inline void subscribe_test() {
       setup_t config{
           {},//pubs
           {"tcp://127.0.0.1:5555"},//subs
           {},//reqs
           {} //reps
       };

       zprocess_t proc{ config };
       z1 temp{};

       while (true) {
           proc.subscribe();
           proc.from_buffer(temp);
           std::cout << "z1 { " << temp.m1.fg << " : " << temp.m1.sd
               << " : " << temp.m2.size() << " } \n\n";
       }
       
   }

}