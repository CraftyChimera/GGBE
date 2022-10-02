//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "../CPU/Cpu.hpp"
#include "../Cartridge/Cartridge.hpp"

class Console {
    Cpu cpu;
    array<byte, memory_map_size> memory;
    bool ram_enabled;
    Cartridge cartridge;
    byte rom_bank_number, ram_bank_number, mode_flag;
    size_t number_of_rom_banks, number_of_ram_banks;

public:
    Console();

    void boot(vector<byte> &data);

    void write(word &address, byte value);

    void loop();

    byte read(word &address);
};

#endif //DMGB_CONSOLE_HPP