#include <iostream>
#include <memory>
#include <string>

template <typename T, typename TT>
std::unique_ptr<T> make_unique_ptr(const TT &val)
{
    return std::make_unique<T>(val);
}

class MyClass
{
public:
    MyClass(const std::string &name) : name_(name)
    {
        std::cout << "MyClass constructed: " << name_ << std::endl;
    }
    ~MyClass()
    {
        std::cout << "MyClass destructed: " << name_ << std::endl;
    }
    void greet() const
    {
        std::cout << "Hello from " << name_ << "!" << std::endl;
    }

private:
    std::string name_;
};

class AnotherClass
{
public:
    AnotherClass(int data) : data_(data)
    {
        std::cout << "AnotherClass constructed with data: " << data_ << std::endl;
    }
    ~AnotherClass()
    {
        std::cout << "AnotherClass destructed with data: " << data_ << std::endl;
    }
    void showData() const
    {
        std::cout << "Data: " << data_ << std::endl;
    }

private:
    int data_;
};

int main()
{
    auto myObject = make_unique_ptr<MyClass>("TestObject");
    myObject->greet();

    auto anotherObject = make_unique_ptr<AnotherClass>(42);
    anotherObject->showData();
    return 0;
}