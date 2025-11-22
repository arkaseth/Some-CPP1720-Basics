#include <iostream>
#include <memory>
#include <string>

struct SafeNode
{
    std::string name;
    std::shared_ptr<SafeNode> next; // owning forward link
    std::weak_ptr<SafeNode> prev;   // non-owning backward link breaks cycles
    SafeNode(const std::string &n) : name(n) { std::cout << "SafeNode " << name << " created\n"; }
    ~SafeNode() { std::cout << "SafeNode " << name << " destroyed\n"; }
};

int main()
{
    std::shared_ptr<SafeNode> x = std::make_shared<SafeNode>("X");
    std::shared_ptr<SafeNode> y = std::make_shared<SafeNode>("Y");
    std::weak_ptr<SafeNode> wx = x;

    // x -> y (owning), y -> x (non-owning)
    x->next = y;
    y->prev = x;

    x.reset();
    y.reset();
    // Both SafeNode destructors will run here, breaking the cycle.
    std::cout << "After resetting x and y, wx expired? " << (wx.expired() ? "yes" : "no")
              << " (no cycle, so destruction occurs)\n";
    return 0;
}