//
// Created by drake on 23/10/22.
//

#ifndef DMGB_PIXEL_MAPPER_HPP
#define DMGB_PIXEL_MAPPER_HPP

#include "GPU_Utility.hpp"

class MMU;

struct Pixel_Info {
    bool is_sprite;
    int color_id;
    int palette;
    int sprite_priority;
    bool background_priority;
};

class Pixel_Mapper {

public:
    explicit Pixel_Mapper(MMU *mmu);

    std::array<hex_codes, screen_width> current_scanline;

    byte fetcher_x, fetcher_y;

    void operator()(int cycles);

    State advance_scan_line();

private:
    MMU *mem_ptr;
    std::deque<Pixel_Info> background_pixel_queue;

    void get_current_background_pixels();

    [[nodiscard]] vector<vector<byte>> current_pixel_data_fetch_address(byte tile_id, byte tile_map_bit) const;

    hex_codes get_hex_from_pixel(Pixel_Info pixel_data);

    //TODO LCD_Control
};

#endif //DMGB_PIXEL_MAPPER_HPP
