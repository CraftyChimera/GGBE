//
// Created by drake on 1/10/22.
//

#ifndef GGBE_GPU_HPP
#define GGBE_GPU_HPP

#include "PixelMapper.hpp"
#include "SDL.h"
#include <fstream>

class MMU;

class GPU {

public:
    SDL_Texture *texture;
    SDL_Window *window;
    SDL_Renderer *renderer;

private:
    std::array<std::array<hex_codes, screen_width>, screen_height> pixels;
    char *formatted_pixels;

    MMU *mem_ptr;
    State current_ppu_state;
    PixelMapper mapper;

    int cycle_delay;
    int cycles_accumulated;

public:
    explicit GPU(MMU *mmu);

    void tick(int cycles);

    ~GPU();

private:

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
