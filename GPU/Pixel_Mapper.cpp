//
// Created by drake on 23/10/22.
//

#include <cassert>
#include <algorithm>
#include "Pixel_Mapper.hpp"
#include "../MMU/Mmu.hpp"

Pixel_Mapper::Pixel_Mapper(MMU *mem) : current_scanline{}, mem_ptr(mem) {
    windows_enabled = false;
    window_encountered = false;
    window_incremented = false;
    is_in_window = false;

    first_read = false;

    fetcher_x = 0x00;
    fetcher_y = 0x90;

    window_x = 0x00;
    window_line_counter = -1;
    mem_ptr->write(ly_address, fetcher_y);
    lcd_reg = 0;
    scroll_offset = 0;
}

State Pixel_Mapper::advance_scan_line() {
    background_pixel_queue.clear();
    fetcher_x = 0;
    window_x = 0;
    fetcher_y++;
    is_in_window = false;

    //Update LY value
    mem_ptr->write(ly_address, fetcher_y % 154);

    if (fetcher_y == screen_height_with_pseudo_scan_lines) {
        fetcher_y = 0;
        return State::OAM_SCAN;
    }

    if (fetcher_y == screen_height) {
        byte interrupt_flags = mem_ptr->read(if_address);
        interrupt_flags = interrupt_flags | (1 << 0);
        mem_ptr->write(if_address, interrupt_flags);
    }

    if (fetcher_y >= screen_height) {
        window_line_counter = -1;
        window_encountered = false;
        return State::V_BLANK;
    }
    first_read = false;

    return State::OAM_SCAN;
}

void Pixel_Mapper::operator()(int cycles) {
    if (fetcher_x == 0)
        scroll_offset = (mem_ptr->read(scx_address)) % 8;

    byte wx = mem_ptr->read(wx_address);

    check_if_window_enabled();

    while (cycles > 0) {
        cycles--;

        if (fetcher_x + window_x == screen_width)
            return;

        if (background_pixel_queue.empty())
            get_current_background_pixels();

        while (scroll_offset > 0) {
            scroll_offset--;
            background_pixel_queue.pop_front();
        }

        bool should_be_in_window = windows_enabled && (fetcher_x + 7 >= wx);
        if (!is_in_window && should_be_in_window) // Transition into window
        {
            window_line_counter++;
            background_pixel_queue.clear();
            is_in_window = true;
            get_current_background_pixels();
        }

        current_scanline.at(fetcher_x + window_x) = get_hex_from_pixel(background_pixel_queue.front());

        if (is_in_window)
            window_x++;
        else
            fetcher_x++;

        background_pixel_queue.pop_front();
    }

    lcd_reg = mem_ptr->read(lcd_control_address);
}

void Pixel_Mapper::get_current_background_pixels() {
    constexpr word tile_map_block_size = 0x0800;
    bool window_tile_map_area_bit = lcd_reg & (1 << 6);
    bool tile_data_area_bit = lcd_reg & (1 << 4);
    bool bg_tile_map_area_bit = lcd_reg & (1 << 3);

    word tile_map_base_addr = (bg_tile_map_area_bit) ? 0x9C00 : 0x9800;

    if (is_in_window)
        tile_map_base_addr = (window_tile_map_area_bit) ? 0x9C00 : 0x9800;

    word tile_data_start_address = (tile_data_area_bit) ? 0x8000 : 0x8800;
    word object_size = 8;

    byte scx = mem_ptr->read(scx_address);
    byte scy = mem_ptr->read(scy_address);

    byte tile_x = ((scx / 8) + (fetcher_x / 8)) & 0x1F;
    byte tile_y = ((scy + fetcher_y) & 0xFF) / 8;

    if (is_in_window) {
        tile_x = window_x / 8;
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

    if (is_in_window)
        y_offset = window_line_counter % 8;

    word address_of_low_byte = start_of_tile_address + 2 * y_offset;

    array<byte, 4> address_of_color_bytes{};
    for (int idx = 0; idx < 4; idx++)
        address_of_color_bytes[idx] = mem_ptr->read(address_of_low_byte + idx);


    for (int pixel_id = 0; pixel_id < object_size; pixel_id++) {
        Pixel_Info current_pixel{};
        auto byte_id = 2 * (pixel_id / 8);
        auto pixel_offset = pixel_id % 8;

        byte low_byte = address_of_color_bytes.at(byte_id);
        byte high_byte = address_of_color_bytes.at(byte_id + 1);

        assert((pixel_offset < 8) && (pixel_offset > -1));

        byte high_bit_at_pos = (bool) (high_byte & (1 << (7 - pixel_offset)));
        byte low_bit_at_pos = (bool) (low_byte & (1 << (7 - pixel_offset)));
        byte current_pixel_color_number = (high_bit_at_pos << 1) | low_bit_at_pos;

        current_pixel.is_sprite = false;
        current_pixel.color_id = current_pixel_color_number;
        background_pixel_queue.push_back(current_pixel);
    }
}

hex_codes Pixel_Mapper::get_hex_from_pixel(Pixel_Info pixel_data) {
    word color_data = mem_ptr->read(bgp_palette_address);

    auto color_id = pixel_data.color_id;
    if ((lcd_reg & (1 << 0)) == 0)
        return color_map.at(0);

    assert((color_id < 4) && (color_id > -1));

    auto color_index = (color_data >> (2 * color_id)) & 0x3;
    return color_map.at(color_index);
}

void Pixel_Mapper::check_if_window_enabled() {
    bool windows_enable_bit = lcd_reg & (1 << 5);

    byte wy = mem_ptr->read(wy_address);

    window_encountered = window_encountered || (wy == fetcher_y);
    windows_enabled = window_encountered && windows_enable_bit;
}