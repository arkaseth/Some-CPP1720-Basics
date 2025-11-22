// nrvo-move.cpp
// Small demo showing NRVO (named return value optimization) vs explicit move vs prvalue.
// Compile with a modern compiler (C++17 or newer) to see guaranteed elision of prvalues.
//
// Example output (C++17):
// 1) NRVO example
//   Constructed(1): make_nrvo
// 2) return std::move(o) (no NRVO, move)
//   Constructed(2): make_move_return
//   Moved(3 <- 2): make_move_return -> make_move_return (moved)
// 3) return prvalue
//   Constructed(4): prvalue
//
// Notes:
// - make_nrvo returns a local named variable. In many compilers NRVO will elide copies/moves.
//   Behavior can differ pre-C++17; with C++17 prvalue/rvalue elision is guaranteed for direct
//   prvalue returns, but NRVO for named locals is still an optimization (commonly applied).
// - Returning std::move(local) inhibits NRVO and forces a move-construction into the caller.

#include <iostream>
#include <string>
#include <utility>

struct Obj {
    static int counter;
    int id;
    std::string name;

    Obj() : id(++counter), name("unnamed") {
        std::cout << "  DefaultConstructed(" << id << "): " << name << '\n';
    }

    explicit Obj(std::string n) : id(++counter), name(std::move(n)) {
        std::cout << "  Constructed(" << id << "): " << name << '\n';
    }

    // copy constructor
    Obj(const Obj& other) : id(++counter), name(other.name + " (copied)") {
        std::cout << "  Copied(" << id << " <- " << other.id << "): " << name << '\n';
    }

    // move constructor
    Obj(Obj&& other) noexcept : id(++counter), name(std::move(other.name)) {
        std::cout << "  Moved(" << id << " <- " << other.id << "): " << name << " (moved)\n";
        other.name = "moved-from";
    }

    ~Obj() {
        std::cout << "  Destructed(" << id << "): " << name << '\n';
    }
};

int Obj::counter = 0;

Obj make_nrvo() {
    // named local; compiler may apply NRVO and construct directly into the caller's storage.
    Obj o("make_nrvo");
    return o; // NRVO often applies -> no copy or move
}

Obj make_move_return() {
    // returning std::move(o) typically inhibits NRVO and will move the object
    Obj o("make_move_return");
    return std::move(o); // forces move (if elision not applied)
}

Obj make_prvalue() {
    // returning a prvalue (temporary) - in C++17 this is constructed directly in caller (guaranteed elision)
    return Obj("prvalue");
}

int main() {
    std::cout << "1) NRVO example\n";
    {
        Obj a = make_nrvo();
    }
    std::cout << '\n';

    std::cout << "2) return std::move(o) (no NRVO, move)\n";
    {
        Obj b = make_move_return();
    }
    std::cout << '\n';

    std::cout << "3) return prvalue\n";
    {
        Obj c = make_prvalue();
    }
    std::cout << '\n';

    return 0;
}