// smart-pointers.cpp
// Examples of unique_ptr, shared_ptr, weak_ptr, and (conditionally) auto_ptr.
// Compile with a modern C++ compiler. auto_ptr example is only compiled for pre-C++17 modes.

#include <iostream>
#include <memory>
#include <string>

struct Widget {
    std::string name;
    Widget(const std::string& n) : name(n) { std::cout << "Widget " << name << " created\n"; }
    ~Widget() { std::cout << "Widget " << name << " destroyed\n"; }
    void greet() const { std::cout << "Hello from " << name << "\n"; }
};

int main() {
    std::cout << "== unique_ptr examples ==\n";
    // create with make_unique (C++14+)
    auto up1 = std::make_unique<Widget>("unique1");
    up1->greet();

    // transfer ownership with std::move
    std::unique_ptr<Widget> up2 = std::move(up1);
    if (!up1) std::cout << "up1 is now null after move\n";
    up2->greet();

    // custom deleter example
    auto deleter = [](Widget* p) {
        std::cout << "Custom deleter called for " << (p ? p->name : "null") << "\n";
        delete p;
    };
    std::unique_ptr<Widget, decltype(deleter)> up3(new Widget("unique_custom"), deleter);
    up3->greet();

    std::cout << "\n== shared_ptr examples ==\n";
    auto sp1 = std::make_shared<Widget>("shared1");
    std::cout << "sp1 use_count: " << sp1.use_count() << "\n";
    {
        auto sp2 = sp1; // shared ownership
        std::cout << "after copy, sp1 use_count: " << sp1.use_count() << "\n";
        sp2->greet();
    }
    std::cout << "after sp2 scope exit, sp1 use_count: " << sp1.use_count() << "\n";

    // reset and aliasing
    auto sp3 = sp1;
    sp3.reset(); // decreases reference count
    std::cout << "after reset sp3, sp1 use_count: " << sp1.use_count() << "\n";

    std::cout << "\n== weak_ptr example ==\n";
    std::weak_ptr<Widget> wp = sp1; // observe, does not contribute to ref count
    std::cout << "wp expired? " << (wp.expired() ? "yes" : "no") << "\n";
    if (auto locked = wp.lock()) {
        std::cout << "locked from weak_ptr, use_count: " << locked.use_count() << "\n";
        locked->greet();
    }

    // destroy the last shared owner
    sp1.reset();
    std::cout << "after resetting sp1, wp expired? " << (wp.expired() ? "yes" : "no") << "\n";
    if (auto locked = wp.lock()) {
        locked->greet();
    } else {
        std::cout << "cannot lock weak_ptr - object gone\n";
    }

    // Cyclic reference example with shared_ptrs (can cause leaks)
    struct CycleNode
    {
        std::string name;
        std::shared_ptr<CycleNode> partner; // owning pointer -> can form cycle
        CycleNode(const std::string &n) : name(n) { std::cout << "CycleNode " << name << " created\n"; }
        ~CycleNode() { std::cout << "CycleNode " << name << " destroyed\n"; }
    };

    {
        auto a = std::make_shared<CycleNode>("A");
        auto b = std::make_shared<CycleNode>("B");
        std::weak_ptr<CycleNode> wa = a; // observe

        // form a cycle: a->partner -> b and b->partner -> a (both shared_ptr)
        a->partner = b;
        b->partner = a;

        a.reset();
        b.reset();
        // Because of the cycle, the CycleNode destructors will not run here.
        std::cout << "After resetting a and b, wa expired? " << (wa.expired() ? "yes" : "no")
                  << " (cycle prevents destruction)\n";
    }

    // Break cycle using weak_ptr for the back-reference
    struct SafeNode
    {
        std::string name;
        std::shared_ptr<SafeNode> next; // owning forward link
        std::weak_ptr<SafeNode> prev;   // non-owning backward link breaks cycles
        SafeNode(const std::string &n) : name(n) { std::cout << "SafeNode " << name << " created\n"; }
        ~SafeNode() { std::cout << "SafeNode " << name << " destroyed\n"; }
    };

    {
        auto x = std::make_shared<SafeNode>("X");
        auto y = std::make_shared<SafeNode>("Y");
        std::weak_ptr<SafeNode> wx = x;

        // x -> y (owning), y -> x (non-owning)
        x->next = y;
        y->prev = x;

        x.reset();
        y.reset();
        // Now objects are destroyed because prev is weak and doesn't keep the reference count alive.
        std::cout << "After resetting x and y, wx expired? " << (wx.expired() ? "yes" : "no")
                  << " (no cycle, objects destroyed)\n";
    }

#if __cplusplus < 201703L
    // auto_ptr was deprecated and removed in C++17. Only compile this block for older standards.
    std::cout << "\n== auto_ptr (pre-C++17) example ==\n";
    {
        std::auto_ptr<Widget> ap1(new Widget("auto1"));
        std::cout << "ap1 points to: " << (ap1.get() ? ap1->name : "null") << "\n";
        // copy transfers ownership (ap1 becomes null)
        std::auto_ptr<Widget> ap2 = ap1;
        std::cout << "after copy, ap1 is " << (ap1.get() ? "not null" : "null") << ", ap2 points to: "
                  << (ap2.get() ? ap2->name : "null") << "\n";
    }
#else
    (void)0; // no-op for C++17+
#endif

    return 0;
}