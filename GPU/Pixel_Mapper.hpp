//
// Created by drake on 23/10/22.
//

#ifndef DMGB_PIXEL_MAPPER_HPP
#define DMGB_PIXEL_MAPPER_HPP

#include "GPU_Utility.hpp"

class MMU;

//TODO:(CGB)sprite priority
struct Pixel_Info {
    bool is_sprite;
    int color_id;
    int palette;
    bool background_priority;
};

class Pixel_Mapper {

public:
    explicit Pixel_Mapper(MMU *mmu);

    std::array<hex_codes, screen_width> current_scanline;

    byte fetcher_x, fetcher_y;

    byte window_line_counter;

    byte lcd_reg;

    void operator()(int cycles);

    State advance_scan_line();

private:
    bool windows_enabled;
    bool window_encountered;
    MMU *mem_ptr;
    std::deque<Pixel_Info> background_pixel_queue;

    byte scroll_offset;

    bool first_read;

    void get_current_background_pixels(bool fetch_window);

    hex_codes get_hex_from_pixel(Pixel_Info pixel_data);

    void check_for_window();
};

#endif //DMGB_PIXEL_MAPPER_HPP
