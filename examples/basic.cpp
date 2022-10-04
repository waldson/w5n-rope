#include <iostream>
#include <w5n/Rope.hpp>

int main()
{
    w5n::Rope rope;
    std::cout << "Basic Operations" << std::endl;

    rope.append("World");
    std::cout << "Append: " << rope.toString() << std::endl;

    rope.prepend("Hello, ");
    std::cout << "Prepend: " << rope.toString() << std::endl;

    rope.erase(1, 7);
    std::cout << "Erase: " << rope.toString() << std::endl;

    rope.insert(1, "ello, W");
    std::cout << "Insert: " << rope.toString() << std::endl;

    auto ch = rope.at(7);
    std::cout << "At: " << ch << std::endl;

    rope.clear();
    std::cout << "Clear: " << rope.toString() << std::endl;
}
