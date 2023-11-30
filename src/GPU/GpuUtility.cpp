//
// Created by drake on 5/2/23.
//
#include "GpuUtility.hpp"

PpuFlags::PpuFlags(byte flag_data) {
    cgb_palette_number = flag_data & 0x7;
    vram_bank = flag_data & (1 << 3);
    dmg_palette_number = flag_data & (1 << 4);
    x_flip = flag_data & (1 << 5);
    y_flip = flag_data & (1 << 6);
    bg_window_over_obj = flag_data & (1 << 7);
}

PpuFlags::PpuFlags() {
    cgb_palette_number = 0;
    vram_bank = 0;
    dmg_palette_number = 0;
    x_flip = 0;
    y_flip = 0;
    bg_window_over_obj = 0;
}