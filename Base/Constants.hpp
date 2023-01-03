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

constexpr auto lcd_control = 0xFF40;
constexpr auto scy_address = 0xFF42;
constexpr auto scx_address = 0xFF43;
constexpr auto ly_address = 0xFF44;

constexpr auto bgp_palette = 0xFF47;

constexpr auto wy_address = 0xFF4A;
constexpr auto wx_address = 0xFF4B;
#endif //DMGB_CONSTANTS_HPP