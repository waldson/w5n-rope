#include <iostream>
#include <w5n/Rope.hpp>

int main()
{
    w5n::Rope rope;
    std::cout << "Replacing emoji:" << std::endl;
    rope.append("Hello😁");
    rope.append("World");

    std::cout << rope.toString() << '\n'; // Hello😁World

    rope.erase(5, 1);
    rope.insert(5, ", ");

    std::cout << rope.toString() << '\n'; // Hello, World
    std::cout << "-----------" << std::endl;

    rope.clear();

    rope.append("😁😂😃😄😅");
    std::cout << "String: " << rope.toString() << std::endl;
    std::cout << "Graphemes Count: " << rope.charCount() << std::endl;
    std::cout << "Size in bytes: " << rope.size() << std::endl;
    std::cout << "----------" << std::endl;
    std::cout << "Rendering them individually:" << std::endl;
    for (size_t i = 0; i < rope.charCount(); ++i) {
        std::cout << rope.at(i) << std::endl;
    }
}
