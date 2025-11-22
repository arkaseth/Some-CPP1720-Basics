#include <iostream>
#include <algorithm> // copy, swap
#include <utility>   // move
#include <cstddef>   // size_t

using namespace std;

class Ruleof5
{
private:
    size_t size_;
    int *data_;

public:
    // Constructor
    Ruleof5(size_t size = 0)
        : size_(size), data_(size ? new int[size] : nullptr)
    {
        cout << "Constructor: size=" << size_ << " data=" << static_cast<const void *>(data_) << endl;
    }

    // Destructor
    ~Ruleof5()
    {
        cout << "Destructor: size=" << size_ << " data=" << static_cast<const void *>(data_) << endl;
        delete[] data_;
    }

    // Copy Constructor
    Ruleof5(const Ruleof5 &other)
        : size_(other.size_), data_(other.size_ ? new int[other.size_] : nullptr)
    {
        cout << "Copy Constructor from data=" << static_cast<const void *>(other.data_) << endl;
        if (data_)
            copy(other.data_, other.data_ + size_, data_);
    }

    // Copy Assignment Operator
    Ruleof5 &operator=(const Ruleof5 &other)
    {
        cout << "Copy Assignment from data=" << static_cast<const void *>(other.data_) << endl;
        if (this == &other)
            return *this;
        Ruleof5 tmp(other); // may throw
        swap(*this, tmp);
        return *this;
    }

    // Move Constructor
    Ruleof5(Ruleof5 &&other) noexcept
        : size_(other.size_), data_(other.data_)
    {
        cout << "Move Constructor stealing data=" << static_cast<const void *>(other.data_) << endl;
        other.size_ = 0;
        other.data_ = nullptr;
    }

    // Move Assignment Operator
    Ruleof5 &operator=(Ruleof5 &&other) noexcept
    {
        cout << "Move Assignment stealing data=" << static_cast<const void *>(other.data_) << endl;
        if (this != &other)
        {
            delete[] data_;
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }
};

int main()
{
    Ruleof5 b1(10);  // Constructor
    Ruleof5 b2 = b1; // Copy Constructor

    Ruleof5 b3;
    b3 = b1; // Copy Assignment

    Ruleof5 b4 = move(b1); // Move Constructor
    Ruleof5 b5;
    b5 = move(b2); // Move Assignment
    return 0;
}