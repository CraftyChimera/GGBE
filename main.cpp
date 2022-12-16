//
// Created by drake on 24/8/22.
//

#include "Console/Console.hpp"
#include "Base/Parser.hpp"

int main(int argv, char **argc) {
    Console x;
    if (argv != 2) {
        std::cout << "Usage: ./program_name path_to_rom";
        return -1;
    }

    std::string path(argc[1]);
    auto data = read_file(path);

    x.run(data);
    return 0;
}

