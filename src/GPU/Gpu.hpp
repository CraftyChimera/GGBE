//
// Created by drake on 1/10/22.
//

#ifndef GGBE_GPU_HPP
#define GGBE_GPU_HPP

#include "PixelMapper.hpp"
#include "SDL.h"
#include <fstream>

class Bus;

class GPU {

public:
    SDL_Texture *texture;
    SDL_Window *window;
    SDL_Renderer *renderer;

private:
    bool lcd_on;
    std::array<std::array<hex_codes, screen_width>, screen_height> pixels;
    char *formatted_pixels;

    Bus *ptr_to_bus;
    State current_ppu_state;
    PixelMapper mapper;

    int cycles_accumulated;

public:
    explicit GPU(Bus *mmu);

    void tick(int cycles);

    ~GPU();

private:
    void check_and_get_registers();

    void init_screen();

    void draw_screen();

    void state_dispatch(int cycles);

    void scan_sprites();

    void advance_scanline();

    void change_stat_state();

    void change_stat_lyc();

    void check_and_raise_stat_interrupts();

    void raise_interrupts();
};

#endif //GGBE_GPU_HPP
