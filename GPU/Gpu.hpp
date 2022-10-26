//
// Created by drake on 1/10/22.
//

#ifndef DMGB_GPU_HPP
#define DMGB_GPU_HPP

#include "Pixel_Mapper.hpp"
#include "SDL.h"

class Console;

class Sprite;

class GPU {

private:
    static constexpr auto oam_start = 0xFE00;

    bool sprites_fetched;

    int cycles_accumulated, cycles_delayed;

    State current_ppu_state;

    std::array<std::array<hex_codes, screen_width>, screen_height> pixels;
    char *formatted_pixels;
    SDL_Surface *native_surface;

    Console *game;
    SDL_Window *display_window;
    Pixel_Mapper mapper;

public:

    explicit GPU(Console *game) noexcept;

    void update(int cycles);

    void resize();

    ~GPU();

private:

    void scan_sprites();

    void draw_screen();
};

#endif //DMGB_GPU_HPP
