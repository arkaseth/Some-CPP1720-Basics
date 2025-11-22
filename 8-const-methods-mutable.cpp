#include <iostream>

class MyClass
{
private:
    int value;
    mutable int accessCount; // mutable allows modification in const methods
public:
    MyClass(int val) : value(val), accessCount(0) {}

    int getValue() const
    {
        accessCount++; // modifying mutable member
        return value;
    }

    int getAccessCount() const
    {
        return accessCount;
    }

    void setValue(int val)
    {
        value = val;
    }

    int &getValueRef()
    {
        return value; // non-const method returning non-const reference
    }

    const int &getValueRefConst() const
    {
        return value; // const method returning const reference
    }
};

int main()
{
    MyClass obj(10);

    std::cout << "Initial Value: " << obj.getValue() << std::endl;
    std::cout << "Access Count: " << obj.getAccessCount() << std::endl;

    obj.setValue(20);
    std::cout << "Updated Value: " << obj.getValue() << std::endl;
    std::cout << "Access Count: " << obj.getAccessCount() << std::endl;

    int &ref = obj.getValueRef();
    ref = 30; // Modifying value through non-const reference
    std::cout << "Modified Value via Ref: " << obj.getValue() << std::endl;

    const int &constRef = obj.getValueRefConst();
    std::cout << "Value via Const Ref: " << constRef << std::endl;

    return 0;
}