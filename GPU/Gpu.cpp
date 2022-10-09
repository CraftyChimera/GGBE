//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"
#include "../Console/Console.hpp"

word GPU::LCD_C = 0xFF40;
word GPU::SCX = 0xFF42;
word GPU::SCY = 0xFF43;


GPU::GPU(Console *game) noexcept: tile_data(std::array<std::array<Object, 128>, 3>{}),
                                  tile_map(std::array<std::array<byte, 0x400>, 2>{}),
                                  game(game),
                                  stale(false) {}

void GPU::fill(std::array<std::array<byte, 0x100>, 0x100> &tile_map_pixels, Object &object_to_load, byte object_x_pos,
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

void GPU::render(std::array<byte, memory_map_size> &memory) {

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

}