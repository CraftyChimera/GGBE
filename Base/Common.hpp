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
#include <variant>

using std::vector;
using std::array;
using std::function;
using byte = uint8_t;
using s_byte = int8_t;
using word = uint16_t;
using d_word = uint32_t;
using std::string;

inline constexpr auto rom_block_size = 0x4000;
inline constexpr auto ram_block_size = 0x2000;
inline constexpr auto memory_map_size = 0x10000;

#endif //DMGB_COMMON_HPP