#include <iostream>
#include <type_traits>
#include <utility>

// Demonstrates C++ reference-collapsing and forwarding references (T&&).
// Compile with: g++ -std=c++17 10-reference-collapsing-rules.cpp

// https://www.geeksforgeeks.org/cpp/reference-collapsing-rules-in-cpp/
// https://lemire.me/blog/2024/05/13/forwarding-references-in-c/

void take(int&)        { std::cout << "take(int&)\n"; }        // binds non-const lvalues
void take(const int&)  { std::cout << "take(const int&)\n"; }  // binds const lvalues and rvalues
void take(int&&)       { std::cout << "take(int&&)\n"; }       // binds rvalues

template<typename T>
void f(T&& x) {
    // T is the deduced template parameter; x is the function parameter.
    // Reference-collapsing happens during deduction:
    // - call with lvalue int  -> T becomes int&, parameter type = int& && -> collapses to int&
    // - call with rvalue int  -> T becomes int,  parameter type = int&&
    // - call with const lvalue int -> T becomes const int&, parameter type = const int&

    std::cout << "Inside f:\n";
    std::cout << "  is_lvalue_reference<T>: " << std::boolalpha << std::is_lvalue_reference<T>::value << '\n';
    std::cout << "  is_rvalue_reference<T>: " << std::boolalpha << std::is_rvalue_reference<T>::value << '\n';
    std::cout << "  is_lvalue_reference<decltype(x)>: " << std::boolalpha
              << std::is_lvalue_reference<decltype(x)>::value << '\n';
    std::cout << "  is_rvalue_reference<decltype(x)>: " << std::boolalpha
              << std::is_rvalue_reference<decltype(x)>::value << '\n';

    std::cout << "  calling take(x): ";
    take(x); // x is always an lvalue expression, so overload resolution sees an lvalue

    std::cout << "  calling take(std::forward<T>(x)): ";
    take(std::forward<T>(x)); // preserves the original value category (forwarding)
    std::cout << '\n';
}

int main() {
    int a = 10;
    const int ca = 20;

    std::cout << "Call with lvalue int a:\n";
    f(a);              // expect: x is int&; forward -> lvalue overload

    std::cout << "Call with const lvalue ca:\n";
    f(ca);             // expect: x is const int&; forward -> const lvalue overload

    std::cout << "Call with rvalue literal 30:\n";
    f(30);             // expect: x is int&&; forward -> rvalue overload

    std::cout << "Call with std::move(a):\n";
    f(std::move(a));   // expect: x is int&&; forward -> rvalue overload

    return 0;
}