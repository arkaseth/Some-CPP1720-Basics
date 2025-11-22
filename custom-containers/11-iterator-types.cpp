// /d:/Projects/arista-interview-prep/custom-containers/11-iterator-types.cpp
// Demonstrate iterator categories: input, forward, bidirectional, random access.
// Shows compile-time checks, category printing and algorithms that require each category.

#include <iostream>
#include <vector>
#include <list>
#include <forward_list>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <type_traits>

template <typename Iter>
void print_category_name(const char* label) {
    using Cat = typename std::iterator_traits<Iter>::iterator_category;
    std::cout << label << ": ";
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, Cat>)
        std::cout << "random_access";
    else if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag, Cat>)
        std::cout << "bidirectional";
    else if constexpr (std::is_base_of_v<std::forward_iterator_tag, Cat>)
        std::cout << "forward";
    else if constexpr (std::is_base_of_v<std::input_iterator_tag, Cat>)
        std::cout << "input";
    else if constexpr (std::is_base_of_v<std::output_iterator_tag, Cat>)
        std::cout << "output";
    else
        std::cout << "unknown";
    std::cout << '\n';
}

int main() {
    // Compile-time assertions
    static_assert(std::is_base_of_v<std::input_iterator_tag,
                  typename std::iterator_traits<std::istream_iterator<int>>::iterator_category>,
                  "istream_iterator should be an input iterator");
    static_assert(std::is_base_of_v<std::forward_iterator_tag,
                  typename std::iterator_traits<std::forward_list<int>::iterator>::iterator_category>,
                  "forward_list::iterator should be a forward iterator");
    static_assert(std::is_base_of_v<std::bidirectional_iterator_tag,
                  typename std::iterator_traits<std::list<int>::iterator>::iterator_category>,
                  "list::iterator should be bidirectional");
    static_assert(std::is_base_of_v<std::random_access_iterator_tag,
                  typename std::iterator_traits<std::vector<int>::iterator>::iterator_category>,
                  "vector::iterator should be random access");

    // 1) Input iterator example: read ints from a stream (single-pass)
    std::istringstream ss("10 20 30");
    std::istream_iterator<int> in_it(ss), in_end;
    print_category_name<decltype(in_it)>("istream_iterator");
    std::cout << "istream values: ";
    std::copy(in_it, in_end, std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';

    // 2) Forward iterator example: std::forward_list works with algorithms requiring ForwardIterator
    std::forward_list<int> fl = {1, 1, 2, 2, 3, 3};
    print_category_name<decltype(fl.begin())>("forward_list::iterator");
    auto new_end = std::unique(fl.begin(), fl.end()); // unique requires ForwardIterator
    std::cout << "forward_list after unique (up to new_end): ";
    for (auto it = fl.begin(); it != new_end; ++it) std::cout << *it << " ";
    std::cout << '\n';

    // 3) Bidirectional iterator example: std::list supports reverse (requires BidirectionalIterator)
    std::list<int> lst = {1, 2, 3, 4, 5};
    print_category_name<decltype(lst.begin())>("list::iterator");
    std::reverse(lst.begin(), lst.end()); // requires BidirectionalIterator
    std::cout << "list after reverse: ";
    for (int x : lst) std::cout << x << " ";
    std::cout << '\n';

    // 4) Random access iterator example: std::vector supports sort (requires RandomAccessIterator)
    std::vector<int> v = {3, 1, 4, 1, 5, 9};
    print_category_name<decltype(v.begin())>("vector::iterator");
    std::sort(v.begin(), v.end()); // requires RandomAccessIterator
    std::cout << "vector after sort: ";
    for (int x : v) std::cout << x << " ";
    std::cout << '\n';

    return 0;
}