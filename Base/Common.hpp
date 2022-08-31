//
// Created by drake on 24/8/22.
//

#ifndef DMGB_COMMON_HPP
#define DMGB_COMMON_HPP

#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>

using std::vector;
using std::array;
using byte = uint8_t;
using half_word = uint16_t;
using std::string;
enum struct Reg {
    a = 0,
    f = 1,
    b = 2,
    c = 3,
    d = 4,
    e = 5,
    h = 6,
    l = 7
};

enum struct Flag {
    z = 7,
    n = 6,
    h = 5,
    c = 4
};
constexpr auto rom_block_size = 0x4000;
constexpr auto ram_block_size = 0x2000;
constexpr auto memory_map_size = 0x10000;

#endif //DMGB_COMMON_HPP