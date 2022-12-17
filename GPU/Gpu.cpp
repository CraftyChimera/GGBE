//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"
#include "Console.hpp"

GPU::GPU(MMU *mem) noexcept: sprites_fetched(false), cycles_accumulated(0), cycles_delayed(0),
                             pixels{}, mapper(mem) {
    native_surface = nullptr;
    mem_ptr = mem;

    int flags = SDL_INIT_VIDEO;

    if (SDL_Init(flags) < 0) {
        std::cout << "Init\n" << SDL_GetError() << "\n";
        return;
    }

    display_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width,
                                      screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (display_window == nullptr) {
        std::cout << "Window" << SDL_GetError() << "\n";
        return;
    }

    int channels = 3, native_width = 160, native_height = 144;

    formatted_pixels = new char[native_width * native_height * channels];

    for (int index = 0; index < native_width * native_height * 3; index++)
        formatted_pixels[index] = 0xF;

    current_ppu_state = State::OAM_SCAN;
}

void GPU::update(int cycles) {

    cycles_accumulated += cycles;

    if (cycles_delayed > 0) {
        int cycles_left = cycles_delayed - cycles;
        cycles = std::max(0, -cycles_left);
        cycles_delayed = std::max(0, cycles_left);

        if (cycles_left > 0)
            return;
    }

    switch (current_ppu_state) {

        case State::OAM_SCAN: {

            if (!sprites_fetched) {
                scan_sprites();
                sprites_fetched = true;
            }

            if (cycles_accumulated >= 80) {
                current_ppu_state = State::DRAW_PIXELS;
                // cycles_delayed += 12;//TODO: incorporate Window and start_of_scanline_delay
            }

            break;
        }

        case State::DRAW_PIXELS: {
            mapper(cycles);

            if (mapper.fetcher_x == screen_width)
                current_ppu_state = State::H_BLANK;

            break;
        }

        default:
            break;
    }

    if (cycles_accumulated >= 456) {

        sprites_fetched = false;
        cycles_accumulated -= 456;

        if (current_ppu_state == State::H_BLANK) {
            pixels.at(mapper.fetcher_y) = mapper.current_scanline;
        }

        current_ppu_state = mapper.advance_scan_line();
        mem_ptr->write(0xFF44, mapper.fetcher_y);

        if (mapper.screen_drawn) {
            mapper.screen_drawn = false;
            draw_screen();
            SDL_Delay(17);
        }

    }

}

GPU::~GPU() {
    delete[] formatted_pixels;
    SDL_DestroyWindow(display_window);
    SDL_Quit();
}

void GPU::scan_sprites() {
    auto object_size = 8;
    std::size_t max_sprites_per_scan_line = 10;
    auto sprite_count = 40;
    byte index_in_loaded_sprites = 0;

    auto &sprites_loaded_ref = mapper.sprites_loaded;
    auto &sprite_position_map_ref = mapper.sprite_position_map;
    auto line_y = mapper.fetcher_y;


    for (int sprite_id = 0; sprite_id < sprite_count; sprite_id++) {
        word sprite_data_start_address = oam_start + 4 * sprite_id;

        byte flags = mem_ptr->read(sprite_data_start_address);
        byte tile_index = mem_ptr->read(sprite_data_start_address + 1);

        byte sprite_x = mem_ptr->read(sprite_data_start_address + 2);
        int sprite_y = mem_ptr->read(sprite_data_start_address + 3);
        sprite_y -= 16;

        if (sprites_loaded_ref.size() == max_sprites_per_scan_line) // 10 sprites have already been drawn
            return;

        if (sprite_y <= line_y && line_y < sprite_y + object_size) {
            sprites_loaded_ref.push_back({tile_index, PPU_flags(flags)});
            sprite_position_map_ref.emplace_back(std::make_pair(sprite_x, index_in_loaded_sprites));
            index_in_loaded_sprites++;
        }
    }
    std::sort(sprite_position_map_ref.begin(), sprite_position_map_ref.end());
}

void GPU::draw_screen() {
    int channels = 3;
    for (auto y_co_ordinate = 0; y_co_ordinate < screen_height; y_co_ordinate++) {
        for (auto x_co_ordinate = 0; x_co_ordinate < screen_width; x_co_ordinate++) {
            auto current_index = (screen_width * y_co_ordinate + x_co_ordinate) * 3;

            formatted_pixels[current_index] = static_cast<char>(pixels[y_co_ordinate][x_co_ordinate] & 0xFF);
            formatted_pixels[current_index + 1] = formatted_pixels[current_index];
            formatted_pixels[current_index + 2] = formatted_pixels[current_index];
        }
    }
    native_surface = SDL_CreateRGBSurfaceFrom((void *) formatted_pixels,
                                              screen_width,
                                              screen_height,
                                              channels * 8,
                                              screen_width * channels,
                                              0x0000FF,
                                              0x00FF00,
                                              0xFF0000,
                                              0);


    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(display_window), nullptr);
    SDL_UpdateWindowSurface(display_window);
}

void GPU::resize() {
    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(display_window), nullptr);
    SDL_UpdateWindowSurface(display_window);
}