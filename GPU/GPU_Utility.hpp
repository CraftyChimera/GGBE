//
// Created by drake on 24/10/22.
//

#ifndef GGBE_UTILITY_GPU_HPP
#define GGBE_UTILITY_GPU_HPP

#include "../Base/Constants.hpp"
#include <deque>

using hex_codes = int;
static constexpr auto screen_height = 144;
static constexpr auto screen_width = 160;
static constexpr auto screen_height_with_pseudo_scan_lines = 154;

static constexpr auto oam_duration_in_t_cycles = 80;
static constexpr auto line_duration_in_t_cycles = 456;

inline constexpr std::array<hex_codes, 4> color_map{0xe0f8d0, 0x88c070, 0x346856, 0x081820};

enum State {
    H_BLANK = 0,
    V_BLANK = 1,
    OAM_SCAN = 2,
    DRAW_PIXELS = 3
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

#endif //GGBE_UTILITY_GPU_HPP
