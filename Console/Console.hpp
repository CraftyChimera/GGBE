//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "../CPU/Cpu.hpp"
#include "../Cartridge/Cartridge.hpp"
#include "../GPU/Gpu.hpp"
#include "SDL.h"

class Console {
    Cpu cpu;
    std::array<byte, memory_map_size> memory;

    bool ram_enabled;
    Cartridge cartridge;
    GPU renderer;

    byte rom_bank_number, ram_bank_number, mode_flag;
    size_t number_of_rom_banks, number_of_ram_banks;

    SDL_Window *window;

public:
    Console();

    ~Console();

    void init(vector<byte> &data);

    void init_cartridge(vector<byte> &data);

    void init_renderer();

    void loop();

    void run(vector<byte> &data);

    void write(word &address, byte value);

    byte read(word &address);
};

#endif //DMGB_CONSOLE_HPP