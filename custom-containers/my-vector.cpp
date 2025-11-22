#include <iostream>
#include <memory>

template <typename T>
class MyVector
{
private:
    T *data;
    size_t capacity;
    size_t size;

    void resize()
    {
        capacity = (capacity == 0) ? 1 : capacity * 2;
        T *newData = new T[capacity];
        for (size_t i = 0; i < size; ++i)
        {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
    }

public:
    MyVector() : data(nullptr), capacity(0), size(0) {}

    ~MyVector()
    {
        delete[] data;
    }

    void push_back(const T &value)
    {
        if (size == capacity)
        {
            resize();
        }
        data[size++] = value;
    }

    void pop_back()
    {
        if (size > 0)
        {
            --size;
        }
    }

    T &operator[](size_t index)
    {
        return data[index];
    }

    size_t getSize() const
    {
        return size;
    }

    size_t getCapacity() const
    {
        return capacity;
    }

    bool isEmpty() const
    {
        return size == 0;
    }

    void clear()
    {
        size = 0;
    }

    void print() const
    {
        for (size_t i = 0; i < size; ++i)
        {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    MyVector(const MyVector &other) : data(nullptr), capacity(other.capacity), size(other.size)
    {
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i)
        {
            data[i] = other.data[i];
        }
    }

    MyVector &operator=(const MyVector &other)
    {
        if (this != &other)
        {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i)
            {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    MyVector(MyVector &&other) noexcept : data(other.data), capacity(other.capacity), size(other.size)
    {
        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }

    MyVector &operator=(MyVector &&other) noexcept
    {
        if (this != &other)
        {
            delete[] data;
            data = other.data;
            capacity = other.capacity;
            size = other.size;
            other.data = nullptr;
            other.capacity = 0;
            other.size = 0;
        }
        return *this;
    }
};

int main()
{
    MyVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.print(); // Output: 1 2 3

    vec.pop_back();
    vec.print(); // Output: 1 2

    std::cout << "Size: " << vec.getSize() << ", Capacity: " << vec.getCapacity() << std::endl;

    MyVector<int> vec2 = vec; // Copy constructor
    vec2.print();             // Output: 1 2

    MyVector<int> vec3;
    vec3 = std::move(vec);                                                          // Move assignment
    vec3.print();                                                                   // Output: 1 2
    std::cout << "Original vector size after move: " << vec.getSize() << std::endl; // Output: 0

    return 0;
}