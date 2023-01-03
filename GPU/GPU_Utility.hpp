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

static constexpr auto oam_duration_in_t_cycles = 80;
static constexpr auto line_duration_in_t_cycles = 456;

inline constexpr std::array<hex_codes, 4> color_map{0x0f380f, 0x306230, 0x8bac0f, 0x9bbc0f};

enum State {
    OAM_SCAN = 2,
    DRAW_PIXELS = 3,
    H_BLANK = 0,
    V_BLANK = 1
};

#endif //DMGB_UTILITY_GPU_HPP
