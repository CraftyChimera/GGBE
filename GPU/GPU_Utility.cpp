//
// Created by drake on 5/2/23.
//
#include "GPU_Utility.hpp"

PPU_flags::PPU_flags(byte flag_data) {
    palette_number = flag_data & (1 << 4);
    x_flip = flag_data & (1 << 5);
    y_flip = flag_data & (1 << 6);
    bg_window_over_obj = flag_data & (1 << 7);
}