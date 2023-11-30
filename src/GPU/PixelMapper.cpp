//
// Created by drake on 23/10/22.
//

#include <algorithm>
#include "PixelMapper.hpp"
#include "../Bus/Bus.hpp"

PixelMapper::PixelMapper(Bus *mem) : current_scanline{}, mem_ptr(mem) {
    windows_enabled = false;
    window_encountered = false;
    is_in_window = false;

    background_x = 0x00;
    fetcher_x = 0x00;
    fetcher_y = 0x90;

    window_x = 0x00;
    window_line_counter = -1;
    mem_ptr->write(ly_address, fetcher_y);
    lcd_reg = 0;
    scroll_offset = 0;

    background_palette_to_color_map.fill(0);
    obj_palette_to_color_map.fill(0);
}

State PixelMapper::advance_scan_line() {
    background_pixel_queue.clear();
    sprite_pixel_queue.clear();
    sprites_loaded.clear();
    sprite_position_map.clear();

    background_x = 0;
    window_x = 0;
    fetcher_x = 0;
    fetcher_y++;
    is_in_window = false;

    //Update LY value
    mem_ptr->write(ly_address, fetcher_y % 154);

    if (fetcher_y == screen_height_with_pseudo_scan_lines) {
        fetcher_y = 0;
        return State::OAM_SCAN;
    }

    if (fetcher_y == screen_height) {
        byte interrupt_flags = mem_ptr->read_current_vram_bank(if_address);
        interrupt_flags = interrupt_flags | (1 << 0);
        mem_ptr->write(if_address, interrupt_flags);

        window_line_counter = -1;
        window_encountered = false;
    }

    if (fetcher_y >= screen_height)
        return State::V_BLANK;

    return State::OAM_SCAN;
}

void PixelMapper::operator()(int cycles) {

    if (fetcher_x == 0) {
        scroll_offset = mem_ptr->read_current_vram_bank(scx_address) % 8;
        background_x = scroll_offset;
        window_x = scroll_offset;
    }

    check_if_window_enabled();
    while (cycles > 0) {
        cycles--;

        if (fetcher_x == screen_width)
            return;

        if (background_pixel_queue.empty())
            get_current_background_pixels();

        if (fetcher_x == 0) {
            background_pixel_queue = std::deque<PixelInfo>(background_pixel_queue.begin() + scroll_offset,
                                                           background_pixel_queue.end());
        }

        check_and_transition_into_window();
        check_and_load_pixels();
        current_scanline.at(fetcher_x++) = get_hex_from_pixel(get_mixed_pixel());

        if (is_in_window)
            window_x++;
        else
            background_x++;
    }
    lcd_reg = mem_ptr->read_current_vram_bank(lcd_control_address);
}

hex_codes PixelMapper::get_hex_from_pixel(PixelInfo pixel_data) {
    int color_data_used = -1;

    if (!mem_ptr->is_cgb) {
        constexpr std::array<hex_codes, 4> color_map = {0xe0f8d0, 0x88c070, 0x346856, 0x081820};

        if (pixel_data.is_sprite) {
            switch (pixel_data.palette) {
                case 0:
                    color_data_used = mem_ptr->read_current_vram_bank(obp0_palette_address);
                    break;
                case 1:
                    color_data_used = mem_ptr->read_current_vram_bank(obp1_palette_address);
                    break;
            }
        } else {
            color_data_used = mem_ptr->read_current_vram_bank(bgp_palette_address);

            if ((lcd_reg & (1 << 0)) == 0)
                return color_map.at(0);
        }

        auto color_id = pixel_data.color_id;
        auto color_index = (color_data_used >> (2 * color_id)) & 0x3;
        return color_map.at(color_index);
    } else {
        int palette_number = pixel_data.palette;
        int color_id = pixel_data.color_id;
        int offset = palette_number * 8 + color_id * 2;
        byte byte1, byte2;
        if (pixel_data.is_sprite)
            byte1 = obj_palette_to_color_map.at(offset), byte2 = obj_palette_to_color_map.at(offset + 1);
        else
            byte1 = background_palette_to_color_map.at(offset), byte2 = background_palette_to_color_map.at(offset + 1);

        auto get_color_in_888_format = [&]() {
            auto r = byte1 & 0x1F;
            auto g = ((byte1 >> 5) & 0x7) | (byte2 & 0x3) << 3;
            auto b = (byte2 >> 2) & 0x1F;

            r = (r << 3) | (r >> 2);
            g = (g << 3) | (g >> 2);
            b = (b << 3) | (b >> 2);
            return (r << 16) | (g << 8) | b;
        };

        return get_color_in_888_format();
    }
}

void PixelMapper::check_if_window_enabled() {
    bool windows_enable_bit = lcd_reg & (1 << 5);

    byte wy = mem_ptr->read_current_vram_bank(wy_address);

    window_encountered = window_encountered || (wy == fetcher_y);
    windows_enabled = window_encountered && windows_enable_bit;
}

void PixelMapper::get_current_background_pixels() {
    PpuFlags flag_data;
    constexpr word tile_map_block_size = 0x0800;
    bool window_tile_map_area_bit = lcd_reg & (1 << 6);
    bool tile_data_area_bit = lcd_reg & (1 << 4);
    bool bg_tile_map_area_bit = lcd_reg & (1 << 3);

    word tile_map_base_addr = (bg_tile_map_area_bit) ? 0x9C00 : 0x9800;

    if (is_in_window)
        tile_map_base_addr = (window_tile_map_area_bit) ? 0x9C00 : 0x9800;

    word tile_data_start_address = (tile_data_area_bit) ? 0x8000 : 0x8800;

    byte scx = mem_ptr->read_current_vram_bank(scx_address);
    byte scy = mem_ptr->read_current_vram_bank(scy_address);

    byte tile_x = (scx / 8 + background_x / 8) & 0x1F;
    byte tile_y = ((scy + fetcher_y) & 0xFF) / 8;

    if (is_in_window) {
        tile_x = window_x / 8;
        tile_y = window_line_counter / 8;
    }

    word current_tile_base_addr = tile_map_base_addr + tile_x + 32 * tile_y;
    auto tile_num = mem_ptr->read_current_vram_bank(current_tile_base_addr);
    if (mem_ptr->is_cgb)
        flag_data = PpuFlags(mem_ptr->read_other_vram_bank(current_tile_base_addr));

    auto block_id = tile_num / 128;
    auto block_offset = tile_num % 128;
    word start_of_tile_address =
            tile_data_start_address + block_id * tile_map_block_size + 16 * block_offset;

    if (!tile_data_area_bit)
        start_of_tile_address =
                tile_data_start_address + (1 - block_id) * tile_map_block_size + 16 * block_offset;

    auto y_offset = (scy + fetcher_y) % 8;

    if (is_in_window)
        y_offset = window_line_counter % 8;

    if (flag_data.y_flip)
        y_offset = 7 - y_offset;

    word address_of_low_byte = start_of_tile_address + 2 * y_offset;
    word address_of_high_byte = address_of_low_byte + 1;

    for (int pixel_id = 0; pixel_id < 8; pixel_id++) {
        PixelInfo current_pixel{};

        byte low_byte = mem_ptr->read_vram_bank(address_of_low_byte, flag_data.vram_bank);
        byte high_byte = mem_ptr->read_vram_bank(address_of_high_byte, flag_data.vram_bank);
        auto pixel_bitmask = 7 - pixel_id;
        if (flag_data.x_flip)
            pixel_bitmask = pixel_id;

        byte high_bit_at_pos = (bool) (high_byte & (1 << (pixel_bitmask)));
        byte low_bit_at_pos = (bool) (low_byte & (1 << (pixel_bitmask)));
        byte current_pixel_color_number = (high_bit_at_pos << 1) | low_bit_at_pos;

        current_pixel.is_sprite = false;
        current_pixel.color_id = current_pixel_color_number;

        if (mem_ptr->is_cgb) {
            current_pixel.background_priority = flag_data.bg_window_over_obj;
            current_pixel.palette = flag_data.cgb_palette_number;
        }

        background_pixel_queue.push_back(current_pixel);
    }
}

std::deque<SpritePixelInfo> PixelMapper::load_new_sprite_pixels() {
    constexpr word tile_data_start_address = 0x8000;
    bool object_size_bit = lcd_reg & (1 << 2);
    bool sprite_enable_bit = lcd_reg & (1 << 1);

    std::deque<SpritePixelInfo> current_sprite_pixels;

    if (sprite_position_map.empty() || !sprite_enable_bit)
        return current_sprite_pixels;

    auto sprite_id = sprite_position_map.front().second;
    auto sprite_y = sprite_position_map.front().first.sprite_y;

    Sprite current = sprites_loaded.at(sprite_id);

    auto tile_num = current.tile_id;
    auto flag_data = current.flags;

    auto y_offset = (fetcher_y - (sprite_y - 16)) % 8;

    if (flag_data.y_flip) {
        y_offset = 7 - y_offset;
        if (object_size_bit)
            tile_num ^= 0x1;
    }

    auto start_of_tile_address = tile_data_start_address + tile_num * 16;

    word address_of_low_byte = start_of_tile_address + 2 * y_offset;
    word address_of_high_byte = address_of_low_byte + 1;

    for (int pixel_id = 0; pixel_id < 8; pixel_id++) {
        SpritePixelInfo current_pixel{};

        byte low_byte = mem_ptr->read_vram_bank(address_of_low_byte, flag_data.vram_bank);
        byte high_byte = mem_ptr->read_vram_bank(address_of_high_byte, flag_data.vram_bank);

        auto pixel_bitmask = 7 - pixel_id;

        if (flag_data.x_flip)
            pixel_bitmask = pixel_id;

        byte high_bit_at_pos = (bool) (high_byte & (1 << pixel_bitmask));
        byte low_bit_at_pos = (bool) (low_byte & (1 << pixel_bitmask));
        byte current_pixel_color_number = (high_bit_at_pos << 1) | low_bit_at_pos;

        current_pixel.is_sprite = true;
        current_pixel.color_id = current_pixel_color_number;

        current_pixel.background_priority = flag_data.bg_window_over_obj;

        if (!mem_ptr->is_cgb)
            current_pixel.palette = flag_data.dmg_palette_number;
        else
            current_pixel.palette = flag_data.cgb_palette_number;

        current_pixel.index_in_oam = sprite_id;

        current_sprite_pixels.push_back(current_pixel);
    }
    return current_sprite_pixels;
}

void PixelMapper::load_pixels_into_sprite_queue(std::deque<SpritePixelInfo> pixels) {
    if (!mem_ptr->is_cgb) {
        for (std::size_t offset = 0; offset < pixels.size(); offset++) {
            if (offset >= sprite_pixel_queue.size())
                sprite_pixel_queue.push_back(pixels.at(offset));

            else if (sprite_pixel_queue.at(offset).color_id == 0)
                sprite_pixel_queue.at(offset) = pixels.at(offset);
        }
    } else {
        for (std::size_t offset = 0; offset < pixels.size(); offset++) {
            if (offset >= sprite_pixel_queue.size())
                sprite_pixel_queue.push_back(pixels.at(offset));

            else if (sprite_pixel_queue.at(offset).color_id == 0 ||
                     sprite_pixel_queue.at(offset).index_in_oam > pixels.front().index_in_oam)
                sprite_pixel_queue.at(offset) = pixels.at(offset);
        }
    }
}

PixelInfo PixelMapper::get_mixed_pixel() {
    PixelInfo current_background_pixel = background_pixel_queue.front();
    background_pixel_queue.pop_front();

    if (sprite_pixel_queue.empty())
        return current_background_pixel;

    PixelInfo current_sprite_pixel = sprite_pixel_queue.front();
    sprite_pixel_queue.pop_front();

    if (!mem_ptr->is_cgb) {
        if (current_sprite_pixel.color_id == 0)
            return current_background_pixel;

        if (current_sprite_pixel.background_priority) {
            if (current_background_pixel.color_id != 0)
                return current_background_pixel;
        }
        return current_sprite_pixel;
    } else {
        bool oam_priority_bit = current_sprite_pixel.background_priority;
        bool background_priority_bit = current_background_pixel.background_priority;

        if (current_sprite_pixel.color_id == 0)
            return current_background_pixel;

        if (current_background_pixel.color_id == 0)
            return current_sprite_pixel;

        if (((lcd_reg & (1 << 0)) == 0) || (!oam_priority_bit && !background_priority_bit))
            return current_sprite_pixel;

        return current_background_pixel;
    }
}

void PixelMapper::check_and_load_pixels() {
    while (!sprite_position_map.empty() && fetcher_x + 8 >= sprite_position_map.front().first.sprite_x) {

        int sprite_position_end = sprite_position_map.front().first.sprite_x;
        int sprite_position_beg = sprite_position_end - 8;

        auto current_sprite_pixels = load_new_sprite_pixels();

        while (!current_sprite_pixels.empty() && sprite_position_beg < 0) {
            current_sprite_pixels.pop_front();
            sprite_position_beg++;
        }

        load_pixels_into_sprite_queue(current_sprite_pixels);
        sprite_position_map.pop_front();
    }
}

void PixelMapper::check_and_transition_into_window() {
    auto wx = mem_ptr->read_current_vram_bank(wx_address);

    bool should_be_in_window = windows_enabled && (fetcher_x + 7 >= wx);

    if (!is_in_window && should_be_in_window) // Transition into window
    {
        window_line_counter++;
        is_in_window = true;
        background_pixel_queue.clear();
        get_current_background_pixels();
    }
}

void PixelMapper::reset() {
    windows_enabled = false;
    window_encountered = false;
    is_in_window = false;

    background_x = 0x00;
    fetcher_x = 0x00;
    fetcher_y = 0x00;

    window_x = 0x00;
    window_line_counter = -1;
    mem_ptr->write(ly_address, fetcher_y);
    lcd_reg = 0;
    scroll_offset = 0;

    current_scanline = {};
    sprite_position_map = {};
    sprites_loaded = {};
}