#include "Rope.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <uni_algo/ranges.h>

int main(int argc, char* argv[])
{
    using uni::views::drop;
    using uni::views::take;

    Rope data3;
    data3.append("🙋😀😁😂😃ção");
    std::cout << data3.at(0) << std::endl;
    std::cout << data3.at(1) << std::endl;
    std::cout << data3.at(2) << std::endl;
    std::cout << data3.at(3) << std::endl;
    std::cout << data3.at(4) << std::endl;
    std::cout << data3.at(5) << std::endl;
    std::cout << data3.at(6) << std::endl;

    // {
    //
    //     Rope data;
    //     data.append("Waldson");
    //     data.append("Patricio");
    //     data.append("Nascimento");
    //     data.append("Leandro");
    //     data3 = data;
    //     data.clear();
    // }
    //
    // Rope data2 = data3;
    // data3.append("OK");
    // data3.erase(2, 5);
    // data3.erase(0, 5);
    // data2.append("HMMM");

    // std::string teste = "🙋😀😁😂😃ção";
    // auto range = uni::ranges::utf8_view(teste);
    //
    // auto offset = range | drop(2) | take(5);
    // auto substr = offset | uni::ranges::to_utf8<std::string>();
    //
    // std::cout << teste.size() << std::endl;
    // std::cout << substr << std::endl;

    // std::cout << data3.substring(100, 10) << std::endl;
    // std::cout << data3.to_string() << std::endl;
    // std::cout << data3.to_string() << std::endl;
    // std::cout << data3.substring(20, 10) << std::endl;
    // std::cout << data3.substring(31, 7) << std::endl;
    // std::cout << data3.substring(30, 10) << std::endl;
    // std::cout << data2.to_string() << std::endl;

    // std::cout << data3.to_string() << std::endl;
    // std::cout << data2.size() << std::endl;
    // std::cout << data2.to_string() << std::endl;
    // data3.insert(8, "OITO");
    // std::cout << data3.to_string() << std::endl;

    return 0;
}
