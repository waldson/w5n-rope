#include <iostream>
#include <w5n/Rope.hpp>

int main()
{
    w5n::Rope rope;
    rope.append("Hello😁");
    rope.append("World");
    std::cout << rope.toString() << '\n'; // Hello😁World

    rope.erase(5, 1);
    rope.insert(5, ", ");

    std::cout << rope.toString() << '\n'; // Hello, World
}
