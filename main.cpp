//
// Created by drake on 24/8/22.
//

#include "Console/Console.hpp"
#include "Base/Parser.hpp"


int main(int argv, char **argc) {
    Console x;
    if (argv != 2)
        return -100;
    string path(argc[1]);
    vector<byte> data = read_file(path);
    return 0;
}

