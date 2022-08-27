//
// Created by drake on 24/8/22.
//
#include "Base/Parser.hpp"
#include "Cartridge/Cartridge.hpp"

int main(int argv, char **argc) {
    if (argv != 2)
        return -100;
    string path(argc[1]);
    vector<byte> data = read_file(path);
    return 0;
}

