//
// Created by drake on 23/10/22.
//

#ifndef GGBE_PIXELMAPPER_HPP
#define GGBE_PIXELMAPPER_HPP

#include "GpuUtility.hpp"

class Bus;

// TODO : (CGB)sprite priority
struct PixelInfo {
    bool is_sprite;
    int color_id;
    int palette;
    bool background_priority;
};

struct SpriteData {
    byte sprite_x;
    byte sprite_y;
};

class PixelMapper {

public:
    explicit PixelMapper(Bus *mmu);

    std::array<hex_codes, screen_width> current_scanline;

    byte fetcher_x;
    byte fetcher_y;

    std::deque<std::pair<SpriteData, byte>> sprite_position_map;
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

    Bus *mem_ptr;
    std::deque<PixelInfo> background_pixel_queue;
    std::deque<PixelInfo> sprite_pixel_queue;

    byte scroll_offset;

    void get_current_background_pixels();

    std::deque<PixelInfo> load_new_sprite_pixels();

    void load_pixels_into_sprite_queue(std::deque<PixelInfo> sprite_data);

    PixelInfo get_mixed_pixel();

    hex_codes get_hex_from_pixel(PixelInfo pixel_data);

    void check_if_window_enabled();

    void check_and_load_pixels();

    void check_and_transition_into_window();
};

#endif //GGBE_PIXELMAPPER_HPP
