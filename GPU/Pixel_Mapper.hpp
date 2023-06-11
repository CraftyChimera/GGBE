//
// Created by drake on 23/10/22.
//

#ifndef GGBE_PIXEL_MAPPER_HPP
#define GGBE_PIXEL_MAPPER_HPP

#include "GPU_Utility.hpp"

class MMU;

//TODO:(CGB)sprite priority
struct Pixel_Info {
    bool is_sprite;
    int color_id;
    int palette;
    bool background_priority;
};

struct Sprite_data {
    byte sprite_x;
    byte sprite_y;
};

class Pixel_Mapper {

public:
    explicit Pixel_Mapper(MMU *mmu);

    std::array<hex_codes, screen_width> current_scanline;

    byte fetcher_x;
    byte fetcher_y;

    std::deque<std::pair<Sprite_data, byte>> sprite_position_map;
    std::vector<Sprite> sprites_loaded;

    void operator()(int cycles);

    State advance_scan_line();

private:
    byte window_line_counter;

    byte lcd_reg;

    byte background_x;
    byte window_x;

    bool windows_enabled;
    bool window_encountered;
    bool is_in_window;

    MMU *mem_ptr;
    std::deque<Pixel_Info> background_pixel_queue;
    std::deque<Pixel_Info> sprite_pixel_queue;

    byte scroll_offset;

    void get_current_background_pixels();

    std::deque<Pixel_Info> load_new_sprite_pixels();

    void load_pixels_into_sprite_queue(std::deque<Pixel_Info> sprite_data);

    Pixel_Info get_mixed_pixel();

    hex_codes get_hex_from_pixel(Pixel_Info pixel_data);

    void check_if_window_enabled();

    void check_and_load_pixels();

    void check_and_transition_into_window();
};

#endif //GGBE_PIXEL_MAPPER_HPP
