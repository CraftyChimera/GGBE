//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"
#include "Console.hpp"
#include "SDL.h"

class Sprite {
    byte x_pos, y_pos;
    byte tile_id;
    byte flags;
};

const State &operator++(State &current_state) {
    switch (current_state) {
        case State::OAM_SCAN:
            return current_state = State::DRAW_PIXELS;
        case State::DRAW_PIXELS:
            return current_state = State::H_BLANK;
        case State::H_BLANK:
            return current_state = State::OAM_SCAN;
        default:
            return current_state;
    }
}

GPU::GPU(Console *game) noexcept: sprites_fetched(false), sprites_loaded{}, cycles_left(0), cycles_accumulated(0),
                                  fetcher_x(0), fetcher_y(0), pixels{}, state_duration{}, current_scanline{} {
    this->game = game;
    int flags = SDL_INIT_VIDEO;

    if (SDL_Init(flags) < 0) {
        std::cout << "Init\n" << SDL_GetError() << "\n";
        return;
    }

    display_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width,
                                      screen_height, SDL_WINDOW_SHOWN);
    if (display_window == nullptr) {
        std::cout << "Window" << SDL_GetError() << "\n";
        return;
    }

    state_duration[State::OAM_SCAN] = 80;
    state_duration[State::DRAW_PIXELS] = 172;
    state_duration[State::H_BLANK] = 204;
    state_duration[State::V_BLANK] = 456;

    current_ppu_state = State::OAM_SCAN;
    cycles_left = state_duration[current_ppu_state];
}

void GPU::update(int cycles) {
    cycles_accumulated += cycles;
    cycles_left -= cycles;

    switch (current_ppu_state) {

        case State::OAM_SCAN: {
            if (!sprites_fetched) {
                scan_sprites();
                sprites_fetched = true;
            }
            break;
        }

        case State::DRAW_PIXELS: {
            draw_scanline();
            break;
        }

        default:
            break;
    }

    if (cycles_left <= 0) {
        ++current_ppu_state;

        if (cycles_accumulated >= 456) {
            fetcher_y++;
            cycles_accumulated = 0;

            if (fetcher_y == screen_height) {
                current_ppu_state = State::V_BLANK;
            }

            if (fetcher_y == screen_height_with_pseudo_scan_lines) {
                fetcher_y = 0;
                current_ppu_state = State::OAM_SCAN;
                //TODO:Draw Logic
                SDL_Delay(17);
            }
        }

        cycles_left = state_duration[current_ppu_state];
    }

}

GPU::~GPU() {
    SDL_DestroyWindow(display_window);
    SDL_Quit();
}

void GPU::scan_sprites() {

}

/*void GPU::fill(std::array<std::array<byte, 0x100>, 0x100> &tile_map_pixels, Object &object_to_load, byte object_x_pos,
               byte object_y_pos) {
    for (size_t x_dir = 0; x_dir < 8; x_dir++)
        for (size_t y_dir = 0; y_dir < 8; y_dir++) {
            tile_map_pixels[8 * object_x_pos + x_dir][8 * object_y_pos + y_dir] = object_to_load.get_data(x_dir, y_dir);
        }
}


void GPU::load_data(std::array<byte, memory_map_size> &memory) {
    load_tile_data(memory);
    load_tile_map(memory);
}

void GPU::load_tile_map(std::array<byte, memory_map_size> &memory) {
    constexpr auto tile_map_start = 0x9800, TILE_MAP_COUNT = 2, TILE_MAP_SIZE = 0x400;
    for (auto tile_map_number = 0; tile_map_number < TILE_MAP_COUNT; tile_map_number++) {
        for (auto tile_map_id = 0; tile_map_id < TILE_MAP_SIZE; tile_map_id++) {
            tile_map[tile_map_number][tile_map_id] = memory[tile_map_start + tile_map_number * TILE_MAP_SIZE +
                                                            tile_map_id];
        }
    }
}

void GPU::load_tile_data(std::array<byte, memory_map_size> &memory) {
    constexpr auto vram_start = 0x8000, BLOCK_COUNT = 3, BLOCK_SIZE = 128, OBJECT_SIZE = 16;
    for (auto block_number = 0; block_number < BLOCK_COUNT; block_number++) {
        for (auto block_id = 0; block_id < BLOCK_SIZE; block_id++) {
            tile_data[block_number][block_id].load(memory,
                                                   vram_start + OBJECT_SIZE * (block_number * BLOCK_SIZE + block_id));
        }
    }
}
*/

/*void GPU::update(int cycles) {
    if (stale) {
        load_data(memory);
        stale = false;
    }

    auto LCD_Control = game->read(LCD_C);

    auto bg_tile_map_area_bit = ((LCD_Control & (1 << BG_TILE_MAP_AREA)) > 0);
    auto tile_data_area_bit = ((LCD_Control & (1 << BG_WINDOW_TILE_DATA_AREA)) > 0);
    auto map_area_used = tile_map[bg_tile_map_area_bit];


    auto constexpr tile_map_length_in_tiles = 0x20;
    auto constexpr tile_map_length_in_bytes = 0x100;

    std::array<std::array<byte, tile_map_length_in_bytes>, tile_map_length_in_bytes> tile_map_pixels{};
    std::array<std::array<byte, 160>, 144> view_port{};


    for (auto object_id = 0; object_id < tile_map_length_in_tiles * tile_map_length_in_tiles; object_id++) {
        byte byte_index = map_area_used[object_id], block_index = byte_index & 0x7F, block_number = (
                (byte_index & (1 << 7)) > 0);
        if (tile_data_area_bit == 0)
            block_number = 3 - block_number;

        byte object_x_pos = object_id / tile_map_length_in_tiles, object_y_pos = object_id % tile_map_length_in_tiles;

        Object temp = tile_data[block_number][block_index];
        fill(tile_map_pixels, temp, object_x_pos, object_y_pos);
    }

    auto scx_value = game->read(SCX), scy_value = game->read(SCY);

    for (size_t x_pos = 0; x_pos < 160; x_pos++)
        for (size_t y_pos = 0; y_pos < 144; y_pos++) {
            view_port[x_pos][y_pos] = tile_map_pixels[(scx_value + x_pos) % tile_map_length_in_bytes][
                    (scy_value + y_pos) % tile_map_length_in_bytes];
        }
*/
