//
// Created by drake on 5/2/23.
//
#include "GPU_Utility.hpp"

PPU_flags::PPU_flags(byte flag_data) {
    palette_number = (flag_data >> 4) & 1;
    x_flip = (flag_data >> 5) & 1;
    y_flip = (flag_data >> 6) & 1;
    bg_window_over_obj = (flag_data >> 7) & 1;
}