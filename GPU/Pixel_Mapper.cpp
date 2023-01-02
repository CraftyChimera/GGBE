//
// Created by drake on 23/10/22.
//

#include <algorithm>
#include "Pixel_Mapper.hpp"
#include "../MMU/Mmu.hpp"

Pixel_Mapper::Pixel_Mapper(MMU *mem) : current_scanline{}, mem_ptr(mem) {
    fetcher_x = 0x00;
    fetcher_y = 0x90;
    mem_ptr->write(0xFF44, fetcher_y);
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

        if (fetcher_x == screen_width)
            return;

        if (background_pixel_queue.empty())
            get_current_background_pixels();

        current_scanline.at(fetcher_x++) = get_hex_from_pixel(background_pixel_queue.front());
        background_pixel_queue.pop_front();
    }
}

void Pixel_Mapper::get_current_background_pixels() {
    word map_base_addr = 0x9800;

    byte scx = mem_ptr->read(scx_address);
    byte scy = mem_ptr->read(scy_address);

    byte tile_x = ((scx / 8) + (fetcher_x / 8)) & 0x1F;
    byte tile_y = ((scy + fetcher_y) & 0xFF) / 8;
    word tile_base_addr = map_base_addr + tile_x + 32 * tile_y;

    auto tile_num = mem_ptr->read(tile_base_addr);
    word tile_map_start_address = 0x8000;
    auto y_offset = (scy + fetcher_y) % 8;

    word address_of_low_byte = tile_map_start_address + (16 * tile_num) + (2 * y_offset);
    byte low_byte = mem_ptr->read(address_of_low_byte);
    byte high_byte = mem_ptr->read(address_of_low_byte + 1);

    for (auto pixel_id = 0; pixel_id < 8; pixel_id++) {
        Pixel_Info current_pixel{};
        byte high_bit_at_pos = (bool) (high_byte & (1 << (7 - pixel_id)));
        byte low_bit_at_pos = (bool) (low_byte & (1 << (7 - pixel_id)));
        byte current_pixel_color_number = (high_bit_at_pos << 1) | low_bit_at_pos;

        current_pixel.is_sprite = false;
        current_pixel.color_id = current_pixel_color_number;
        background_pixel_queue.push_back(current_pixel);
    }
}

hex_codes Pixel_Mapper::get_hex_from_pixel(Pixel_Info pixel_data) {
    constexpr word bgp_palette = 0xFF47;
    auto color_id = pixel_data.color_id;
    word color_data = mem_ptr->read(bgp_palette);

    auto color_index = (color_data >> (2 * color_id)) & 0x3;
    return color_map.at(color_index);
}