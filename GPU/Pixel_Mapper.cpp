//
// Created by drake on 23/10/22.
//

#include <algorithm>
#include "Pixel_Mapper.hpp"
#include "../MMU/Mmu.hpp"

Pixel_Mapper::Pixel_Mapper(MMU *mem) : screen_drawn(false), current_scanline{}, fetcher_x(0), fetcher_y(0),
                                       mem_ptr(mem) {}


int Pixel_Mapper::operator()(int cycles) {
    int delay = 0;
    while (cycles > 0) {
        cycles--;
        if (background_pixel_queue.empty())
            get_current_background_pixels();

        if (fetcher_x == screen_width) {
            break;
        }

        fetcher_x++;

        current_scanline.at(fetcher_x++) = get_hex_from_pixel(get_mixed_pixel());
        background_pixel_queue.pop_front();
    }

    if (fetcher_x == screen_width) {
        return delay;
    }

    while (!sprite_position_map.empty() && fetcher_x + 8 >= sprite_position_map.front().first) {

        int sprite_position_end = sprite_position_map.front().first;
        int sprite_position_beg = sprite_position_end - 8;

        auto current_sprite_pixels = get_current_sprite_pixels();

        for (auto sprite_position_iterator = sprite_position_beg;
             sprite_position_iterator != sprite_position_end; sprite_position_iterator++) {
            if (sprite_position_iterator < 0)
                current_sprite_pixels.pop_front();
        }

        load_pixels_into_sprite_queue(std::move(current_sprite_pixels));

        if (!sprite_position_map.empty())
            sprite_position_map.pop_front();

        delay += get_sprite_delay();
    }
    if (!sprite_pixel_queue.empty())
        sprite_pixel_queue.pop_front();

    return delay;
}

State Pixel_Mapper::advance_scan_line() {

    sprites_loaded.clear();
    background_pixel_queue.clear();
    sprite_pixel_queue.clear();

    fetcher_x = 0, fetcher_y++;

    if (fetcher_y == screen_height_with_pseudo_scan_lines) {
        fetcher_y = 0;
        screen_drawn = true;
        return State::OAM_SCAN;
    }

    if (fetcher_y >= screen_height)
        return State::V_BLANK;

    return State::OAM_SCAN;
}

void Pixel_Mapper::get_current_background_pixels() {
    byte tile_map_bit = 1;

    byte scx = mem_ptr->read(scx_address), scy = mem_ptr->read(scy_address);

    byte tile_x = ((scx / 8) + fetcher_x) & 0x1F;
    byte tile_y = ((scy + fetcher_y) & 0xFF) / 8;
    byte tile_id = (tile_x + 32 * tile_y) & 0x3FF;

    auto tile_data = current_pixel_data_fetch_address(tile_id, tile_map_bit);

    auto y_offset = 2 * (fetcher_y + scy) % 8;
    byte low_byte = tile_data.at(y_offset).at(0), high_byte = tile_data.at(y_offset).at(1);

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

std::deque<Pixel_Info> Pixel_Mapper::get_current_sprite_pixels() {
    std::deque<Pixel_Info> current_sprite_pixels;
    if (sprite_position_map.empty())
        return current_sprite_pixels;
    auto sprite_id = sprite_position_map.front().second;
    Sprite current = sprites_loaded[sprite_id];

    auto tile_id = current.tile_id;
    auto flag_data = current.flags;

    byte scy = mem_ptr->read(scy_address);

    auto tile_data = current_pixel_data_fetch_address(tile_id, 1);

    if (flag_data.y_flip)
        std::reverse(tile_data.begin(), tile_data.end());

    auto y_offset = 2 * (fetcher_y + scy) % 8;
    byte low_byte = tile_data[y_offset][0], high_byte = tile_data[y_offset][1];

    for (auto pixel_id = 7, counter = 0; pixel_id > -1; pixel_id--, counter++) {
        Pixel_Info current_pixel{};
        byte high_bit_at_pos = ((high_byte & (1 << pixel_id)) > 0);
        byte low_bit_at_pos = ((low_byte & (1 << pixel_id)) > 0);
        byte current_pixel_color_number = (high_bit_at_pos << 1) + low_bit_at_pos;

        current_pixel.is_sprite = true;
        current_pixel.color_id = current_pixel_color_number;
        current_pixel.background_priority = current.flags.bg_window_over_obj;
        current_pixel.sprite_priority = sprite_id;
        current_pixel.palette = current.flags.palette_number;

        current_sprite_pixels.push_back(current_pixel);
    }
    return current_sprite_pixels;
}

vector<vector<byte>> Pixel_Mapper::current_pixel_data_fetch_address(byte tile_id, byte tile_map_bit) const {
    word tile_map_start_address = 0x8000;
    vector<vector<byte>> tile_data;
    byte object_size = 8;
    constexpr word tile_length = 2;
    constexpr word block_length = 256;
    constexpr word block_height = 8;
    constexpr word block_size = block_length * block_height;

    byte block_id = tile_id / 128;

    if (tile_map_bit == 0) {
        tile_map_start_address = 0x8800;
        block_id = 2 - block_id;
    }

    byte position_within_block = tile_id % 128;
    word x_offset = block_id * block_size + tile_length * position_within_block;
    word start_of_tile = tile_map_start_address + x_offset;

    for (auto row = 0; row < object_size; row++) {
        tile_data.push_back(
                {
                        mem_ptr->read(start_of_tile + row * block_length), //TODO adjust for obj size 16
                        mem_ptr->read(start_of_tile + row * block_length + 1)
                });
    }
    return tile_data;
}

void Pixel_Mapper::load_pixels_into_sprite_queue(std::deque<Pixel_Info> pixels) {
    for (std::size_t offset = 0; offset < pixels.size(); offset++) {
        //TODO: This works for Non CGB. Need a different Logic for CGB

        if (offset >= sprite_pixel_queue.size())
            sprite_pixel_queue.push_back(pixels.at(offset));

        else if (sprite_pixel_queue.at(offset).color_id == 0)
            sprite_pixel_queue.at(offset) = pixels.at(offset);

    }
}

Pixel_Info Pixel_Mapper::get_mixed_pixel() {
    Pixel_Info current_background_pixel = background_pixel_queue.front(), current_sprite_pixel;
    current_sprite_pixel = current_background_pixel;

    if (!sprite_pixel_queue.empty())
        current_sprite_pixel = sprite_pixel_queue.front();

    if (current_sprite_pixel.color_id == 0)
        return current_background_pixel;

    if (current_sprite_pixel.background_priority && current_background_pixel.color_id != 0)
        return current_background_pixel;

    return current_sprite_pixel;
}

int Pixel_Mapper::get_sprite_delay() {
    return 11 - std::min(5, (fetcher_x + mem_ptr->read(scx_address)) % 8);
}

hex_codes Pixel_Mapper::get_hex_from_pixel(Pixel_Info pixel_data) {
    int color_data = -1;

    if (pixel_data.is_sprite) {
        switch (pixel_data.palette) {
            case 0:
                color_data = mem_ptr->read(obj0_palette);
                break;
            case 1:
                color_data = mem_ptr->read(obj1_palette);
                break;
            default:
                break;
        }
    } else {
        color_data = mem_ptr->read(bgp_palette);
    }

    auto color_no = pixel_data.color_id;
    auto color_index = (color_data >> (2 * color_no)) & 0x3;

    return color_map.at(color_index);
}