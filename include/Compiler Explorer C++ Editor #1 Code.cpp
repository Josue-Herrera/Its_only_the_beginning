#include <iostream>

// -------------------------------
// C++03 recursive typelist
// -------------------------------
template <typename H, typename T>
struct typelist {
    typedef H Head;
    typedef T Tail;
};

struct null_typelist {};

// -------------------------------
// C++11 typelist
// -------------------------------
template <typename...>
struct tlist {};

// -------------------------------
// Type functions to convert from
// typelist to tlist
// -------------------------------
template <typename... Ts, typename... Us>
static auto concat_impl(tlist<Ts...>, tlist<Us...>) -> tlist<Ts..., Us...>;

static auto to_tlist_impl(null_typelist) -> tlist<>;

template <typename Head, typename Tail>
static auto to_tlist_impl(typelist<Head, Tail>)
    -> decltype(concat_impl(tlist<Head>{}, to_tlist_impl(Tail{})));

template <typename Typelist>
using to_tlist = decltype(to_tlist_impl(Typelist{}));

// -------------------------------
// Dummy 'foo' function
// -------------------------------
template <typename T>
void print(T&& t) {
    std::cout << t.get() << '\n';
}

template <typename... Args>
void foo(const Args&... args) {
    std::initializer_list<int>{(print(args), 0)...};
}

// -----------------------------------------------------------------------------
struct Float {
    float get() const { return 7.5; }
};
struct Int {
    int get() const { return 1337; }
};
struct String {
    const char* get() const { return "Hello!"; }
};

// ---------------------------------
// Helper to unpack parameters from
// a tlist and pass them in to 'foo'
// ---------------------------------
template <typename... Ts>
void call_foo(tlist<Ts...>) {
    foo(Ts{}...);
}




#include <vector>
int main() {

std::vector<int> tester = { 1, 2 ,3 ,4 ,5 };

}