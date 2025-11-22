/**
 * @file the-rule-of-5.cpp
 * @brief Minimal demonstration of the Rule of Five in C++.
 *
 * This file implements BigBuffer, a small RAII class that owns a heap-allocated
 * int[] buffer and demonstrates:
 *  - constructor (default / sized) to allocate the resource,
 *  - destructor to release the resource,
 *  - copy constructor for deep-copy semantics,
 *  - copy assignment using the copy-and-swap idiom for strong exception safety,
 *  - move constructor and move assignment to "steal" resources efficiently.
 *
 * Class invariants and ownership:
 *  - size_ represents the current length of the buffer;
 *  - data_ is either nullptr (for size_ == 0) or points to a heap array of size_;
 *  - BigBuffer owns the memory pointed to by data_ and is responsible for deleting it.
 *
 * Key design points:
 *  - Deep copy: copy constructor allocates new storage and copies elements so two
 *    BigBuffer objects never share the same underlying pointer.
 *  - Move semantics: move constructor/assignment transfer ownership and leave the
 *    moved-from object in a valid, destructible state (size_ == 0, data_ == nullptr).
 *  - Exception safety: copy assignment is implemented with copy-and-swap — create a
 *    local copy (may throw), then swap. This yields the strong exception guarantee:
 *    either assignment succeeds, or the left-hand side remains unchanged.
 *  - Self-assignment: copy assignment checks for self-assignment (or relies on
 *    copy-and-swap semantics) to remain safe when assigning an object to itself.
 *  - Resource cleanup: destructor deletes the owned array; move operations null out
 *    the source pointer to prevent double-delete.
 *  - noexcept: marking move constructor and move assignment noexcept is recommended
 *    to enable optimizations (e.g., in containers like std::vector) and to convey
 *    that moving cannot throw.
 *
 * Reasons for the Rule of Five:
 *  - If a class manages a resource (memory, file handle, socket, etc.), the default
 *    compiler-generated copy/move operations may lead to shallow copies, double-free
 *    bugs, use-after-free, or leaks. Implementing a custom destructor implies that
 *    special handling of copy/move semantics is necessary.
 *  - The Rule of Five states that if you declare any one of: destructor, copy
 *    constructor, copy assignment, move constructor, or move assignment, you
 *    should consider whether you need to declare the others to ensure correct,
 *    efficient, and safe behavior.
 *  - Implementing move operations allows resources to be transferred cheaply
 *    instead of performing expensive deep copies, improving performance for temporaries
 *    and container operations.
 *  - Implementing copy operations correctly (deep copy) preserves value semantics:
 *    copies are independent and safe to destroy separately.
 *
 * Usage notes:
 *  - This example is intentionally minimal to illustrate ownership mechanics;
 *    in production code prefer standard RAII types like std::vector<int> or
 *    std::unique_ptr<int[]> to manage dynamic arrays safely and succinctly.
 *  - The class is not inherently thread-safe; concurrent access must be synchronized
 *    by the caller.
 *
 * Behavioral summary (high level):
 *  - Constructing allocates (when size > 0).
 *  - Copying allocates a new buffer and copies elements.
 *  - Moving transfers the pointer and resets the source to an empty state.
 *  - Destruction frees the buffer if owned.
 */
// the-rule-of-5.cpp
// Minimal demonstration of the Rule of Five in C++
// Implements a simple heap-owned buffer with deep-copy semantics,
// move semantics (steal the resource), and proper cleanup.

#include <iostream>
#include <algorithm> // std::copy, std::swap
#include <utility>   // std::move
#include <cstddef>   // std::size_t

class BigBuffer {
private:
    std::size_t size_;
    int* data_;

public:
    // 1) Default / sized constructor
    explicit BigBuffer(std::size_t n = 0)
        : size_(n), data_(n ? new int[n] : nullptr)
    {
        std::cout << "ctor(size=" << size_ << ") data=" << static_cast<const void*>(data_) << '\n';
    }

    // 2) Destructor
    ~BigBuffer()
    {
        std::cout << "dtor(size=" << size_ << ") data=" << static_cast<const void*>(data_) << '\n';
        delete[] data_;
    }

    // 3) Copy constructor (deep copy)
    BigBuffer(const BigBuffer& other)
        : size_(other.size_), data_(other.size_ ? new int[other.size_] : nullptr)
    {
        std::cout << "copy ctor from data=" << static_cast<const void*>(other.data_) << '\n';
        if (data_)
            std::copy(other.data_, other.data_ + size_, data_);
    }

    // 4) Copy assignment (copy-and-swap idiom)
    BigBuffer& operator=(const BigBuffer& other)
    {
        std::cout << "copy assign from data=" << static_cast<const void*>(other.data_) << '\n';
        if (this == &other) return *this;
        BigBuffer tmp(other);       // may throw, strong exception safety
        swap(*this, tmp);
        return *this;
    }

    // 5) Move constructor (steal resources) - noexcept recommended
    BigBuffer(BigBuffer&& other) noexcept
        : size_(other.size_), data_(other.data_)
    {
        std::cout << "move ctor stealing data=" << static_cast<const void*>(data_) << '\n';
        other.size_ = 0;
        other.data_ = nullptr;
    }

    // 6) Move assignment (release own, take other's resources) - noexcept recommended
    BigBuffer& operator=(BigBuffer&& other) noexcept
    {
        std::cout << "move assign stealing data=" << static_cast<const void*>(other.data_) << '\n';
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }

    // swap function used by copy-and-swap
    friend void swap(BigBuffer& a, BigBuffer& b) noexcept
    {
        using std::swap;
        swap(a.size_, b.size_);
        swap(a.data_, b.data_);
    }

    // utility methods
    std::size_t size() const { return size_; }

    void fill(int v)
    {
        for (std::size_t i = 0; i < size_; ++i) data_[i] = v;
    }

    void print(const char* name = "") const
    {
        std::cout << (name ? name : "") << " BigBuffer@" << this
                  << " size=" << size_
                  << " data=" << static_cast<const void*>(data_);
        if (data_ && size_ > 0) std::cout << " first=" << data_[0];
        std::cout << '\n';
    }
};

int main()
{
    std::cout << "Create b1\n";
    BigBuffer b1(5);
    b1.fill(7);
    b1.print("b1");

    std::cout << "\nCopy-construct b2 from b1\n";
    BigBuffer b2 = b1;           // copy constructor
    b2.print("b2");

    std::cout << "\nMove-construct b3 from b1\n";
    BigBuffer b3 = std::move(b1); // move constructor
    b3.print("b3");
    b1.print("b1 (after move)");

    std::cout << "\nAssign copy to b2 (self-test)\n";
    b2 = b2;                     // self-assignment safe
    b2.print("b2");

    std::cout << "\nCopy-assign b2 <- b3\n";
    b2 = b3;                     // copy assignment
    b2.print("b2");
    b3.print("b3");

    std::cout << "\nMove-assign b3 <- BigBuffer(3)\n";
    b3 = BigBuffer(3);           // move assignment (temporary moved)
    b3.fill(42);
    b3.print("b3");

    std::cout << "\nEnd of main, destructors will run\n";
    return 0;
}