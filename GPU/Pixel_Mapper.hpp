//
// Created by drake on 23/10/22.
//

#ifndef DMGB_PIXEL_MAPPER_HPP
#define DMGB_PIXEL_MAPPER_HPP

#include "GPU_Utility.hpp"

class Console;

struct Pixel_Info {
    bool is_sprite;
    int color_id;
    int palette;
    int sprite_priority;
    bool background_priority;
};

class Pixel_Mapper {

public:
    explicit Pixel_Mapper(Console *console);

    bool screen_drawn;

    std::deque<std::pair<byte, byte>> sprite_position_map;
    std::vector<Sprite> sprites_loaded;

    std::array<hex_codes, screen_width> current_scanline;

    byte fetcher_x, fetcher_y;

    int operator()(int cycles);

    State advance_scan_line();

private:
    Console *console;
    std::deque<Pixel_Info> background_pixel_queue, sprite_pixel_queue;
    std::deque<Pixel_Info> sprites_in_scanline;
    std::deque<bool> background_priority;

    std::deque<Pixel_Info> get_current_sprite_pixels();

    void get_current_background_pixels();

    void load_pixels_into_sprite_queue(std::deque<Pixel_Info> pixels);

    [[nodiscard]] vector<vector<byte>> current_pixel_data_fetch_address(byte tile_id, byte tile_map_bit) const;

    Pixel_Info get_mixed_pixel();

    int get_sprite_delay();

    hex_codes get_hex_from_pixel(Pixel_Info pixel_data);

    //TODO LCD_Control
};

#endif //DMGB_PIXEL_MAPPER_HPP
