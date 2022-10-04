//
// Created by drake on 24/8/22.
//

#ifndef DMGB_CONSTANTS_HPP
#define DMGB_CONSTANTS_HPP

#include <array>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;

using byte = uint8_t;
using s_byte = int8_t;
using word = uint16_t;
using d_word = uint32_t;

inline constexpr auto rom_block_size = 0x4000;
inline constexpr auto ram_block_size = 0x2000;
inline constexpr auto memory_map_size = 0x10000;

#endif //DMGB_CONSTANTS_HPP