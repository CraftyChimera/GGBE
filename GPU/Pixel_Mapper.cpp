//
// Created by drake on 23/10/22.
//

#include <algorithm>
#include "Pixel_Mapper.hpp"
#include "../MMU/Mmu.hpp"

Pixel_Mapper::Pixel_Mapper(MMU *mem) : current_scanline{}, mem_ptr(mem) {
    windows_enabled = false;
    window_encountered = false;

    fetcher_x = 0x00;
    window_line_counter = -1;
    fetcher_y = 0x90;
    mem_ptr->write(ly_address, fetcher_y);
}

State Pixel_Mapper::advance_scan_line() {
    background_pixel_queue.clear();
    fetcher_x = 0;
    fetcher_y++;

    if (fetcher_y == screen_height_with_pseudo_scan_lines) {
        fetcher_y = 0;
        return State::OAM_SCAN;
    }

    if (fetcher_y >= screen_height) {
        window_line_counter = -1;
        window_encountered = false;
        return State::V_BLANK;
    }

    return State::OAM_SCAN;
}

void Pixel_Mapper::operator()(int cycles) {
    check_for_window();
    bool old_fetch_window = false;
    bool new_fetch_window = false;
    byte wx = mem_ptr->read(wx_address);

    while (cycles > 0) {
        cycles--;
        if (fetcher_x == screen_width)
            return;

        if (!old_fetch_window)
            new_fetch_window = windows_enabled && (fetcher_x >= (wx - 7));

        if (new_fetch_window && !old_fetch_window) {
            background_pixel_queue.clear();
            old_fetch_window = true;
            fetcher_x = 0;
            window_line_counter++;
        }

        if (background_pixel_queue.empty())
            get_current_background_pixels(new_fetch_window);

        current_scanline.at(fetcher_x++) = get_hex_from_pixel(background_pixel_queue.front());
        background_pixel_queue.pop_front();
    }
}

void Pixel_Mapper::get_current_background_pixels(bool fetch_window) {
    constexpr word tile_map_block_size = 0x0800;
    byte lcd_reg = mem_ptr->read(lcd_control);

    bool window_tile_map_area_bit = lcd_reg & (1 << 6);
    bool tile_data_area_bit = lcd_reg & (1 << 4);
    bool bg_tile_map_area_bit = lcd_reg & (1 << 3);
    bool object_size_bit = lcd_reg & (1 << 2);

    word tile_map_base_addr = (bg_tile_map_area_bit) ? 0x9C00 : 0x9800;
    if (fetch_window)
        tile_map_base_addr = (window_tile_map_area_bit) ? 0x9C00 : 0x9800;

    word tile_data_start_address = (tile_data_area_bit) ? 0x8000 : 0x8800;
    word object_size = (object_size_bit) ? 16 : 8;

    byte scx = mem_ptr->read(scx_address);
    byte scy = mem_ptr->read(scy_address);

    byte tile_x = ((scx / 8) + (fetcher_x / 8)) & 0x1F;
    byte tile_y = ((scy + fetcher_y) & 0xFF) / 8;

    if (fetch_window) {
        tile_x = fetcher_x / 8;
        tile_y = window_line_counter / 8;
    }

    word current_tile_base_addr = tile_map_base_addr + tile_x + 32 * tile_y;
    auto tile_num = mem_ptr->read(current_tile_base_addr);

    auto block_id = tile_num / 128;
    auto block_offset = tile_num % 128;
    word start_of_tile_address =
            tile_data_start_address + block_id * tile_map_block_size + 2 * object_size * block_offset;

    if (!tile_data_area_bit)
        start_of_tile_address =
                tile_data_start_address + (1 - block_id) * tile_map_block_size + 2 * object_size * block_offset;

    auto y_offset = (scy + fetcher_y) % 8;
    word address_of_low_byte = start_of_tile_address + (2 * y_offset);

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
    word color_data = mem_ptr->read(bgp_palette);

    auto color_id = pixel_data.color_id;
    auto color_index = (color_data >> (2 * color_id)) & 0x3;
    return color_map.at(color_index);
}

void Pixel_Mapper::check_for_window() {
    byte lcd_reg = mem_ptr->read(lcd_control);
    bool windows_enable_bit = lcd_reg & (1 << 5);

    byte wy = mem_ptr->read(wy_address);

    window_encountered = window_encountered || (wy == fetcher_y);
    windows_enabled = window_encountered && windows_enable_bit;
}