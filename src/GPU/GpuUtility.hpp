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

enum State {
    H_BLANK = 0,
    V_BLANK = 1,
    OAM_SCAN = 2,
    DRAW_PIXELS = 3
};

struct PpuFlags {
    using bit = bool;

    bit bg_window_over_obj;
    bit y_flip;
    bit x_flip;
    bit dmg_palette_number;
    bit vram_bank;
    byte cgb_palette_number;

    PpuFlags();

    explicit PpuFlags(byte);
};

struct Sprite {
    byte tile_id;
    PpuFlags flags;
};

#endif //GGBE_UTILITY_GPU_HPP
