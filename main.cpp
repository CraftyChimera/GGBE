//
// Created by drake on 24/8/22.
//

#include "Console/Console.hpp"
#include "Base/Parser.hpp"

int main([[maybe_unused]] int argv, char **argc) {
    Console x;
    std::string path(argc[1]);
    auto data = read_file(path);
    x.run(data);
    return 0;
}

