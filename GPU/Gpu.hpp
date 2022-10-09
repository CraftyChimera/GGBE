//
// Created by drake on 1/10/22.
//

#ifndef DMGB_GPU_HPP
#define DMGB_GPU_HPP

#include "Utility.hpp"
#include "Objects.hpp"
#include <iostream>

class Console;

class GPU {

    std::array<std::array<Object, 128>, 3> tile_data;
    std::array<std::array<byte, 0x400>, 2> tile_map;
    Console *game;

public:
    bool stale;

    explicit GPU(Console *game) noexcept;

    void render(std::array<byte, memory_map_size> &memory);

private:
    static void fill(std::array<std::array<byte, 0x100>, 0x100> &tile_map_pixels, Object &object_to_load, byte object_x,
                     byte object_y);

    void load_data(std::array<byte, memory_map_size> &memory);

    void load_tile_map(std::array<byte, memory_map_size> &memory);

    void load_tile_data(std::array<byte, memory_map_size> &memory);

    enum LCD_C {
        LCD_PPU_ENABLE = 7,
        WINDOW_TILE_MAP_AREA = 6,
        WINDOWS_ENABLE = 5,
        BG_WINDOW_TILE_DATA_AREA = 4,
        BG_TILE_MAP_AREA = 3,
        OBJ_SIZE = 2,
        OBJ_ENABLE = 1,
        BG_WINDOW_ENABLE = 0 //TODO CGB: PRIORITY IN CGB
    };

    static word LCD_C, SCX, SCY;
};

#endif //DMGB_GPU_HPP
