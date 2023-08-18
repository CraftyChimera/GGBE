//
// Created by drake on 24/8/22.
//

#include "Base/Console.hpp"
#include "Base/Parser.hpp"

int main(int argv, char **argc) {
    if (argv != 2) {
        std::cout << "Usage: ./program_name path_to_rom";
        return -1;
    }

    std::string path(argc[1]);
    auto data = read_file(path);
    Console console(data);
    console.run();
    return 0;
}

