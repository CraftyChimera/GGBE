//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"
#include "Console.hpp"

void GPU::raise_interrupts() {
    byte interrupt_flags = mem_ptr->read(if_address);
    interrupt_flags = interrupt_flags | (1 << 1);
    mem_ptr->write(if_address, interrupt_flags);
}


void GPU::change_stat_state() {
    byte stat_reg = mem_ptr->read(lcd_stat_address);
    stat_reg >>= 2;
    stat_reg <<= 2;
    stat_reg += current_ppu_state;
    mem_ptr->write(lcd_stat_address, stat_reg);
}

void GPU::change_stat_lyc() {
    auto lyc = mem_ptr->read(lyc_address);
    auto ly = mem_ptr->read(ly_address);

    auto stat_reg = mem_ptr->read(lcd_stat_address);
    int bitmask = 0xFF - (1 << 2);
    stat_reg &= bitmask;
    stat_reg |= ((lyc == ly) << 2);

    mem_ptr->write(lcd_stat_address, stat_reg);
}

void GPU::check_and_raise_stat_interrupts() {
    byte lcd_reg = mem_ptr->read(lcd_stat_address);

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

void GPU::init_sdl() {
    int flags = SDL_INIT_VIDEO;
    if (SDL_Init(flags) < 0) return;

    display_window = SDL_CreateWindow("DMGB", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width,
                                      screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (display_window == nullptr)
        return;
}

void GPU::init_screen() {
    int channels = 3;
    int size = screen_height * screen_width * channels;

    formatted_pixels = new char[size];
    memset(formatted_pixels, 0x00, size);

    native_surface = SDL_CreateRGBSurfaceFrom((void *) formatted_pixels, screen_width, screen_height,
                                              channels * 8, screen_width * channels,
                                              0x0000FF, 0x00FF00, 0xFF0000, 0);
}

GPU::GPU(MMU *mem) noexcept: pixels{}, formatted_pixels{}, native_surface{}, display_window{}, mapper(mem) {
    cycles_accumulated = 0;
    mem_ptr = mem;

    init_sdl();
    init_screen();

    current_ppu_state = State::V_BLANK;
    cycle_delay = 0;
}

void GPU::update(int cycles) {
    if (mem_ptr->lyc_written) {
        change_stat_lyc();
        check_and_raise_stat_interrupts();
        mem_ptr->lyc_written = false;
    }
    cycles *= 4;
    cycles_accumulated += cycles;

    if (cycle_delay > 0) {
        auto time_delayed = std::min(cycle_delay, cycles);
        cycles -= time_delayed;
        cycle_delay -= time_delayed;
    }

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
                current_ppu_state = State::DRAW_PIXELS;
                change_stat_state();
                cycle_delay = 12 + oam_duration_in_t_cycles - cycles_accumulated;
                if (cycle_delay < 0)
                    mapper(-cycle_delay);
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
        //get_bg();
        //SDL_Delay(17);
    }
    change_stat_state();
    change_stat_lyc();
    check_and_raise_stat_interrupts();
}

void GPU::draw_screen() {

    for (auto y_co_ordinate = 0; y_co_ordinate < screen_height; y_co_ordinate++) {
        for (auto x_co_ordinate = 0; x_co_ordinate < screen_width; x_co_ordinate++) {
            auto current_index = (screen_width * y_co_ordinate + x_co_ordinate) * 3;

            // color is an entry from color_map
            auto color = pixels.at(y_co_ordinate).at(x_co_ordinate);

            formatted_pixels[current_index + 2] = static_cast<char>( color & 0xFF );
            color >>= 8;
            formatted_pixels[current_index + 1] = static_cast<char>( color & 0xFF );
            color >>= 8;
            formatted_pixels[current_index] = static_cast<char>( color & 0xFF );
        }
    }

    native_surface->pixels = formatted_pixels;
    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(display_window), nullptr);
    SDL_UpdateWindowSurface(display_window);
}

void GPU::get_bg() {
    auto lcd_reg = mapper.lcd_reg;
    bool window_tile_map_area_bit = lcd_reg & (1 << 6);
    bool tile_data_area_bit = lcd_reg & (1 << 4);
    bool bg_tile_map_area_bit = lcd_reg & (1 << 3);
    bool object_size_bit = lcd_reg & (1 << 2);

    auto map_base_addr = (bg_tile_map_area_bit) ? 0x9C00 : 0x9800;;

    auto tile_map_base_addr = (tile_data_area_bit) ? 0x8000 : 0x8800;

    auto tile_cols = 20;
    auto tile_rows = 18;

    for (int tile_y = 0; tile_y < tile_rows; tile_y++) {
        for (auto y_offset = 0; y_offset < 8; y_offset++) {

            for (int tile_x = 0; tile_x < tile_cols; tile_x++) {

                auto tile_base_addr = map_base_addr + (tile_y * 32) + tile_x;
                auto tile_num = mem_ptr->read(tile_base_addr);

                auto fetch_addr = tile_map_base_addr + (tile_num * 16) + (y_offset * 2);

                byte low_byte = mem_ptr->read(fetch_addr);
                byte high_byte = mem_ptr->read(fetch_addr + 1);

                for (auto x_offset = 0; x_offset < 8; x_offset++) {
                    auto low_bit = (bool) (low_byte & (1 << (7 - x_offset)));
                    auto high_bit = (bool) (high_byte & (1 << (7 - x_offset)));
                    auto color_id = (high_bit << 1) | low_bit;
                    pixels.at(8 * tile_y + y_offset).at(8 * tile_x + x_offset) = color_map.at(color_id);
                }
            }
        }
    }
    draw_screen();
}


void GPU::resize() {
    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(display_window), nullptr);
    SDL_UpdateWindowSurface(display_window);
}

GPU::~GPU() {
    SDL_FreeSurface(native_surface);
    delete[] formatted_pixels;
    SDL_DestroyWindow(display_window);
    SDL_Quit();
}