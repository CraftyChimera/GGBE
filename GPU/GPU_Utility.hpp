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

#endif //DMGB_UTILITY_GPU_HPP
