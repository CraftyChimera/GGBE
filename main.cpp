//
// Created by drake on 24/8/22.
//

#include "Console/Console.hpp"
#include "Base/Parser.hpp"

int main([[maybe_unused]] int argv, char **argc) {
    Console x;
    std::string name = "main.cpp";

    std::string path(argc[1]);
    auto data = read_file(path);
    std::array<std::array<int, 5>, 3> tile_data;
    std::cout << tile_data.size();
    return 0;
}

