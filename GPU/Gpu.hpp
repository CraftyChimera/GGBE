//
// Created by drake on 1/10/22.
//

#ifndef DMGB_GPU_HPP
#define DMGB_GPU_HPP

#include "Pixel_Mapper.hpp"
#include "SDL.h"
#include <fstream>

class MMU;

class GPU {

private:
    std::array<std::array<hex_codes, screen_width>, screen_height> pixels;
    char *formatted_pixels;
    SDL_Surface *native_surface;
    SDL_Window *display_window;

    MMU *mem_ptr;
    State current_ppu_state;
    Pixel_Mapper mapper;
    bool first_frame;
    int cycles_accumulated;

public:

    explicit GPU(MMU *mmu) noexcept;

    void update(int cycles);

    void resize();

    // void get_bg();

    ~GPU();

private:

    void init_sdl();

    void init_screen();

    void draw_screen();

    void state_dispatch(int cycles);

    void advance_scanline();

    void change_stat_state();

    void change_stat_lyc();

    void check_and_raise_stat_interrupts();

    void raise_interrupts();
};

#endif //DMGB_GPU_HPP
