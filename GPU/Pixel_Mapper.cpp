//
// Created by drake on 23/10/22.
//

#include <algorithm>
#include "Pixel_Mapper.hpp"
#include "../MMU/Mmu.hpp"

Pixel_Mapper::Pixel_Mapper(MMU *mem) : current_scanline{}, mem_ptr(mem) {
    fetcher_x = 0x00;
    fetcher_y = 0x90;
}

State Pixel_Mapper::advance_scan_line() {
    background_pixel_queue.clear();
    fetcher_x = 0;
    fetcher_y++;

    if (fetcher_y == screen_height_with_pseudo_scan_lines) {
        fetcher_y = 0;
        return State::OAM_SCAN;
    }

    if (fetcher_y >= screen_height)
        return State::V_BLANK;

    return State::OAM_SCAN;
}

void Pixel_Mapper::operator()(int cycles) {
    while (cycles > 0) {
        cycles--;

        if (background_pixel_queue.empty())
            get_current_background_pixels();

        current_scanline.at(fetcher_x++) = get_hex_from_pixel(background_pixel_queue.front());
        background_pixel_queue.pop_front();

        if (fetcher_x == screen_width)
            return;
    }
}

void Pixel_Mapper::get_current_background_pixels() {
    byte tile_map_bit = 1;
    constexpr word scx_address = 0xFF42;
    constexpr word scy_address = 0xFF43;

    byte scx = mem_ptr->read(scx_address);
    byte scy = mem_ptr->read(scy_address);
    byte tile_x = ((scx / 8) + fetcher_x) & 0x1F;
    byte tile_y = ((scy + fetcher_y) & 0xFF) / 8;

    byte tile_id = (tile_x + 32 * tile_y) & 0x03FF;
    auto tile_data = current_pixel_data_fetch_address(tile_id, tile_map_bit);

    auto y_offset = 2 * (fetcher_y + scy) % 8;
    byte low_byte = tile_data.at(y_offset).at(0);
    byte high_byte = tile_data.at(y_offset).at(1);

    for (auto pixel_id = 7; pixel_id > -1; pixel_id--) {
        Pixel_Info current_pixel{};
        byte high_bit_at_pos = (high_byte & (1 << pixel_id)) > 0;
        byte low_bit_at_pos = (low_byte & (1 << pixel_id)) > 0;
        int current_pixel_color_number = (high_bit_at_pos << 1) + low_bit_at_pos;

        current_pixel.is_sprite = false;
        current_pixel.color_id = current_pixel_color_number;
        background_pixel_queue.push_back(current_pixel);
    }
}

vector<vector<byte>> Pixel_Mapper::current_pixel_data_fetch_address(byte tile_id, byte tile_map_bit) const {

    constexpr byte object_height = 8;
    constexpr word tile_length = 2;
    constexpr word block_length = 256;
    constexpr word block_height = 8;
    constexpr word block_size = block_length * block_height;

    vector<vector<byte>> tile_data;
    byte block_id = tile_id / 128;

    word tile_map_start_address = 0x8000;

    if (tile_map_bit == 0) {
        tile_map_start_address = 0x8800;
        block_id = 2 - block_id;
    }

    byte position_within_block = tile_id % 128;

    word x_offset = block_id * block_size + tile_length * position_within_block;
    word start_of_tile = tile_map_start_address + x_offset;

    for (auto row = 0; row < object_height; row++) {
        tile_data.push_back(
                {
                        mem_ptr->read(start_of_tile + row * block_length),
                        mem_ptr->read(start_of_tile + row * block_length + 1)
                });
    }
    return tile_data;
}

hex_codes Pixel_Mapper::get_hex_from_pixel(Pixel_Info pixel_data) {
    constexpr std::array<hex_codes, 4> color_map{0x000000, 0xD3D3D3, 0x5A5A5A, 0xFFFFFF};
    constexpr word bgp_palette = 0xFF47;

    int color_data = mem_ptr->read(bgp_palette);

    std::array<byte, 4> colors{};
    for (int i = 0; i < 4; i++) {
        colors.at(i) = color_data & 0x3;
        color_data >>= 2;
    }

    auto color_index = colors.at(pixel_data.color_id);
    return color_map.at(color_index);
}