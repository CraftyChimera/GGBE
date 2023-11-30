//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"
#include "Console.hpp"
#include <set>

void GPU::raise_interrupts() {
    byte interrupt_flags = ptr_to_bus->read_current_vram_bank(if_address);
    interrupt_flags = interrupt_flags | (1 << 1);
    ptr_to_bus->write(if_address, interrupt_flags);
}


void GPU::change_stat_state() {
    byte stat_reg = ptr_to_bus->read_current_vram_bank(lcd_stat_address);
    stat_reg >>= 2;
    stat_reg <<= 2;
    stat_reg += current_ppu_state;
    ptr_to_bus->write(lcd_stat_address, stat_reg);
}

void GPU::change_stat_lyc() {
    auto lyc = ptr_to_bus->read_current_vram_bank(lyc_address);
    auto ly = ptr_to_bus->read_current_vram_bank(ly_address);

    auto stat_reg = ptr_to_bus->read_current_vram_bank(lcd_stat_address);
    int bitmask = 0xFF - (1 << 2);
    stat_reg &= bitmask;
    stat_reg |= ((lyc == ly) << 2);

    ptr_to_bus->write(lcd_stat_address, stat_reg);
}

void GPU::check_and_raise_stat_interrupts() {
    byte lcd_reg = ptr_to_bus->read_current_vram_bank(lcd_stat_address);

    static bool old_stat = false;
    bool lyc_eq = lcd_reg & (1 << 2);
    bool hblank_bit = lcd_reg & (1 << 3);
    bool vblank_bit = lcd_reg & (1 << 4);
    bool oam_bit = lcd_reg & (1 << 5);
    bool lyc_bit = lcd_reg & (1 << 6);

    bool new_stat = (
            (current_ppu_state == H_BLANK && hblank_bit)
            || (current_ppu_state == V_BLANK && vblank_bit)
            || (current_ppu_state == OAM_SCAN && oam_bit)
            || (lyc_eq && lyc_bit)
    );

    if (!old_stat && new_stat) {
        raise_interrupts();
    }

    old_stat = new_stat;
}

void GPU::init_screen() {
    int channels = 3;
    int size = screen_height * screen_width * channels;

    formatted_pixels = new char[size];
    memset(formatted_pixels, 0x00, size);
}

GPU::GPU(Bus *mmu)
        : texture{}, window{}, renderer{}, lcd_on(false), pixels{}, formatted_pixels{}, mapper(mmu) {
    cycles_accumulated = 0;
    ptr_to_bus = mmu;
    init_screen();

    current_ppu_state = State::V_BLANK;
}

void GPU::check_and_get_registers() {
    if (ptr_to_bus->background_palette_written) {
        byte bcps_reg = ptr_to_bus->read_current_vram_bank(bcps_address);
        auto address = bcps_reg & 0x3F;
        mapper.background_palette_to_color_map.at(address) = ptr_to_bus->read_current_vram_bank(bcpd_address);
        if (bcps_reg & (1 << 7)) {
            address++;
            ptr_to_bus->write(bcps_address, (1 << 7) | (address & 0x3F));
        }
        ptr_to_bus->background_palette_written = false;
    }

    if (ptr_to_bus->obj_palette_written) {
        byte ocps_reg = ptr_to_bus->read_current_vram_bank(ocps_address);
        auto address = ocps_reg & 0x3F;
        mapper.obj_palette_to_color_map.at(address) = ptr_to_bus->read_current_vram_bank(ocpd_address);
        if (ocps_reg & (1 << 7)) {
            address++;
            ptr_to_bus->write(ocps_address, (1 << 7) | (address & 0x3F));
        }

        ptr_to_bus->obj_palette_written = false;
    }

    if (ptr_to_bus->lcdc_write) {
        bool new_lcd_on = ptr_to_bus->read_current_vram_bank(lcd_control_address) & (1 << 7);
        if (!new_lcd_on && lcd_on) {
            cycles_accumulated = 0;
            mapper.reset();
        }
        lcd_on = new_lcd_on;
        ptr_to_bus->lcdc_write = false;
    }

    if (ptr_to_bus->lyc_written) {
        change_stat_lyc();
        check_and_raise_stat_interrupts();
        ptr_to_bus->lyc_written = false;
    }
}

void GPU::tick(int cycles) {
    check_and_get_registers();

    if (!lcd_on)
        return;

    cycles *= 4;
    cycles_accumulated += cycles;
    state_dispatch(cycles);

    if (cycles_accumulated < line_duration_in_t_cycles)
        return;

    cycles_accumulated = 0;
    advance_scanline();
}

void GPU::state_dispatch(int cycles) {
    switch (current_ppu_state) {
        case State::OAM_SCAN: {

            if (cycles_accumulated >= oam_duration_in_t_cycles) {
                scan_sprites();
                current_ppu_state = State::DRAW_PIXELS;
                change_stat_state();
            }
            return;
        }

        case State::DRAW_PIXELS: {
            mapper(cycles);
            if (mapper.fetcher_x == screen_width) {
                current_ppu_state = State::H_BLANK;
                change_stat_state();
                check_and_raise_stat_interrupts();
            }
            return;
        }

        case State::H_BLANK:
        case State::V_BLANK:
        default:
            return;
    }

}


void GPU::advance_scanline() {
    byte &fetcher_y = mapper.fetcher_y;

    if (fetcher_y < screen_height) {
        pixels.at(fetcher_y) = mapper.current_scanline;
    }

    current_ppu_state = mapper.advance_scan_line();

    if (fetcher_y == 0) {
        draw_screen();
        SDL_Delay(10);
    }
    change_stat_state();
    change_stat_lyc();
    check_and_raise_stat_interrupts();
}

void GPU::draw_screen() {
    for (auto y_co_ordinate = 0; y_co_ordinate < screen_height; y_co_ordinate++) {
        for (auto x_co_ordinate = 0; x_co_ordinate < screen_width; x_co_ordinate++) {
            auto current_index = (screen_width * y_co_ordinate + x_co_ordinate) * 3;
            auto color = pixels.at(y_co_ordinate).at(x_co_ordinate);
            formatted_pixels[current_index + 2] = static_cast<char>( color & 0xFF );
            color >>= 8;
            formatted_pixels[current_index + 1] = static_cast<char>( color & 0xFF );
            color >>= 8;
            formatted_pixels[current_index] = static_cast<char>( color & 0xFF );
        }
    }


    SDL_UpdateTexture(texture, nullptr, formatted_pixels, 160 * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void GPU::scan_sprites() {
    byte lcd_control_reg = ptr_to_bus->read_current_vram_bank(lcd_control_address);
    bool object_size_bit = lcd_control_reg & (1 << 2);
    int object_size = object_size_bit ? 16 : 8;

    constexpr std::size_t max_sprites_per_scan_line = 10;
    constexpr auto sprite_count = 40;
    constexpr word oam_start = 0xFE00;
    byte index_in_loaded_sprites = 0;

    auto &sprites_loaded_ref = mapper.sprites_loaded;
    auto &sprite_position_map_ref = mapper.sprite_position_map;
    int line_y = mapper.fetcher_y;

    for (int sprite_id = 0;
         sprite_id < sprite_count && sprites_loaded_ref.size() < max_sprites_per_scan_line; sprite_id++) {
        word sprite_data_start_address = oam_start + 4 * sprite_id;

        byte flags = ptr_to_bus->read_current_vram_bank(sprite_data_start_address + 3);

        byte tile_id = ptr_to_bus->read_current_vram_bank(sprite_data_start_address + 2);
        if (object_size_bit) { tile_id = tile_id & 0xFE; }

        byte sprite_x = ptr_to_bus->read_current_vram_bank(sprite_data_start_address + 1);
        byte sprite_y = ptr_to_bus->read_current_vram_bank(sprite_data_start_address + 0);

        if (sprite_y <= line_y + 16 && line_y + 16 < sprite_y + object_size) {
            if (line_y + 16 >= sprite_y + 8)
                tile_id |= 0x1;
            sprites_loaded_ref.push_back({tile_id, PpuFlags(flags)});

            sprite_position_map_ref.emplace_back(
                    SpriteData{sprite_x, sprite_y}, index_in_loaded_sprites);

            index_in_loaded_sprites++;
        }
    }

    std::sort(sprite_position_map_ref.begin(), sprite_position_map_ref.end(), [&](
            std::pair<SpriteData, byte> &a, std::pair<SpriteData, byte> &b) {
        return a.first.sprite_x < b.first.sprite_x || (a.first.sprite_x == b.first.sprite_x && a.second < b.second);
    });
}

GPU::~GPU() {
    delete[] formatted_pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}