//
// Created by drake on 24/8/22.
//

#ifndef GGBE_CONSTANTS_HPP
#define GGBE_CONSTANTS_HPP

#include <array>
#include <vector>
#include <iostream>
#include <set>
#include <cstdint>

using std::vector;
using std::cout;

using byte = uint8_t;
using s_byte = int8_t;
using word = uint16_t;

constexpr word joypad_reg_address = 0xFF00;

constexpr word div_address = 0xFF04;
constexpr word tima_address = 0xFF05;
constexpr word tma_address = 0xFF06;
constexpr word tac_address = 0xFF07;

constexpr word if_address = 0xFF0F;

constexpr word lcd_control_address = 0xFF40;
constexpr word lcd_stat_address = 0xFF41;
constexpr word scy_address = 0xFF42;
constexpr word scx_address = 0xFF43;
constexpr word ly_address = 0xFF44;
constexpr word lyc_address = 0xFF45;
constexpr word dma_address = 0xFF46;
constexpr word bgp_palette_address = 0xFF47;
constexpr word obp0_palette_address = 0xFF48;
constexpr word obp1_palette_address = 0xFF49;

constexpr word wy_address = 0xFF4A;
constexpr word wx_address = 0xFF4B;

constexpr word key1_address = 0xFF4D;
constexpr word vbk_address = 0xFF4F;

constexpr word hdma1_address = 0xFF51;
constexpr word hdma2_address = 0xFF52;
constexpr word hdma3_address = 0xFF53;
constexpr word hdma4_address = 0xFF54;
constexpr word hdma5_address = 0xFF55;
// constexpr word rp_address = 0xFF56;
constexpr word bcps_address = 0xFF68;
constexpr word bcpd_address = 0xFF69;
constexpr word ocps_address = 0xFF6A;
constexpr word ocpd_address = 0xFF6B;
constexpr word opri_address = 0xFF6C;
constexpr word svbk_address = 0xFF70;

constexpr word ie_address = 0xFFFF;
#endif //GGBE_CONSTANTS_HPP