//
// Created by drake on 24/10/22.
//

#ifndef DMGB_UTILITY_GPU_HPP
#define DMGB_UTILITY_GPU_HPP

#include "../Base/Constants.hpp"
#include <deque>

using hex_codes = int;
static constexpr auto screen_height = 144;
static constexpr auto screen_width = 160;
static constexpr auto screen_height_with_pseudo_scan_lines = 154;

enum State {
    OAM_SCAN = 2,
    DRAW_PIXELS = 3,
    H_BLANK = 0,
    V_BLANK = 1
};

struct PPU_flags {
    using bit = bool;

    bit bg_window_over_obj;
    bit y_flip;
    bit x_flip;
    bit palette_number;

    explicit PPU_flags(byte);
};

struct Sprite {
    byte tile_id;
    PPU_flags flags;
};

enum Color {
    WHITE = 0,
    LIGHT_GREY = 1,
    DARK_GREY = 2,
    BLACK = 3
};

constexpr std::array<hex_codes, 4> color_map{0x000000, 0xD3D3D3, 0x5A5A5A, 0xFFFFFF};

constexpr word scx_address = 0xFF42;
constexpr word scy_address = 0xFF43;

constexpr word bgp_palette = 0xFF47;
constexpr word obj0_palette = 0xFF48;
constexpr word obj1_palette = 0xFF49;

#endif //DMGB_UTILITY_GPU_HPP
