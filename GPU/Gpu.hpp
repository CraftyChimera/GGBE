//
// Created by drake on 1/10/22.
//

#ifndef DMGB_GPU_HPP
#define DMGB_GPU_HPP

#include "Constants.hpp"
#include "SDL.h"
#include <iostream>

class Console;

class Sprite;

enum State {
    OAM_SCAN = 2,
    DRAW_PIXELS = 3,
    H_BLANK = 0,
    V_BLANK = 1
};

const State &operator++(State &);

class GPU {

private:
    static constexpr auto screen_height = 144;
    static constexpr auto screen_width = 160;
    static constexpr auto screen_height_with_pseudo_scan_lines = 154;

    bool sprites_fetched;
    std::vector<Sprite> sprites_loaded;

    int cycles_left, cycles_accumulated;
    int fetcher_x, fetcher_y;

    State current_ppu_state;
    std::array<std::array<byte, screen_width>, screen_height> pixels;

    Console *game;
    SDL_Window *display_window;

    std::array<int, 4> state_duration;
    std::array<byte, screen_width> current_scanline;
public:

    explicit GPU(Console *game) noexcept;

    void update(int cycles);

private:

    ~GPU();

    void scan_sprites();

    void draw_scanline();

    /* std::array<std::array<Object, 128>, 3> tile_data;
    std::array<std::array<byte, 0x400>, 2> tile_map;
    enum LCD_C {
        LCD_PPU_ENABLE = 7,
        WINDOW_TILE_MAP_AREA = 6,
        WINDOWS_ENABLE = 5,
        BG_WINDOW_TILE_DATA_AREA = 4,
        BG_TILE_MAP_AREA = 3,
        OBJ_SIZE = 2,
        OBJ_ENABLE = 1,
        BG_WINDOW_ENABLE = 0 //TODO CGB: PRIORITY IN CGB
    };

    static word LCD_C, SCX, SCY;*/
};

#endif //DMGB_GPU_HPP
