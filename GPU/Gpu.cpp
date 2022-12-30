//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"
#include "Console.hpp"

void GPU::init_sdl() {
    int flags = SDL_INIT_VIDEO;
    if (SDL_Init(flags) < 0) return;

    display_window = SDL_CreateWindow("DMGB", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width,
                                      screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (display_window == nullptr) return;
}

void GPU::init_screen() {
    int channels = 3;
    int native_width = 160;
    int native_height = 144;
    int size = native_height * native_width * channels;

    formatted_pixels = new char[size];
    memset(formatted_pixels, 0x00, size);
}

GPU::GPU(MMU *mem) noexcept: pixels{}, formatted_pixels{}, native_surface{}, display_window{}, mapper(mem) {
    cycles_accumulated = 0;
    mem_ptr = mem;

    init_sdl();
    init_screen();
    mem_ptr->write(0xFF44, 0x90);

    current_ppu_state = State::V_BLANK;
    first_frame = true;
}

void GPU::update(int cycles) {
    cycles_accumulated += cycles;
    state_dispatch(cycles);

    if (cycles_accumulated < 456)
        return;

    cycles_accumulated -= 456;
    byte &fetcher_y = mapper.fetcher_y;

    if (fetcher_y < screen_height)
        pixels.at(fetcher_y) = mapper.current_scanline;

    current_ppu_state = mapper.advance_scan_line();

    // fetcher_y moving back to 0 means frame has been drawn completely except in case of first frame
    if (fetcher_y == 0) {
        if (first_frame) {
            first_frame = false;
            return;
        }

        draw_screen();
        SDL_Delay(17); // wait 1/60th of a sec each time a frame is drawn
    }
}

void GPU::state_dispatch(int cycles) {
    switch (current_ppu_state) {
        case State::OAM_SCAN: {
            if (cycles_accumulated >= 80)
                current_ppu_state = State::DRAW_PIXELS;

            return;
        }

        case State::DRAW_PIXELS: {
            mapper(cycles);

            if (mapper.fetcher_x == screen_width)
                current_ppu_state = State::H_BLANK;
            return;
        }

        case State::H_BLANK:
        case State::V_BLANK:
        default:
            return;
    }
}

void GPU::draw_screen() {
    int channels = 3;

    for (auto y_co_ordinate = 0; y_co_ordinate < screen_height; y_co_ordinate++) {
        for (auto x_co_ordinate = 0; x_co_ordinate < screen_width; x_co_ordinate++) {
            auto current_index = (screen_width * y_co_ordinate + x_co_ordinate) * 3;

            // color is an entry from color_map
            auto color = pixels.at(y_co_ordinate).at(x_co_ordinate);

            formatted_pixels[current_index + 2] = static_cast<char>( color & 0xFF );
            color >>= 2;
            formatted_pixels[current_index + 1] = static_cast<char>( color & 0xFF );
            color >>= 2;
            formatted_pixels[current_index] = static_cast<char>( color & 0xFF );
        }
    }

    native_surface = SDL_CreateRGBSurfaceFrom((void *) formatted_pixels, screen_width, screen_height,
                                              channels * 8, screen_width * channels,
                                              0x0000FF, 0x00FF00, 0xFF0000, 0);


    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(display_window), nullptr);
    SDL_UpdateWindowSurface(display_window);
}

void GPU::resize() {
    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(display_window), nullptr);
    SDL_UpdateWindowSurface(display_window);
}

GPU::~GPU() {
    delete[] formatted_pixels;
    SDL_DestroyWindow(display_window);
    SDL_Quit();
}