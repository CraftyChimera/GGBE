//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "../CPU/Cpu.hpp"
#include "../Cartridge/Cartridge.hpp"

class Console {
    array<byte, memory_map_size> memory;
    bool ram_enabled;
    CPU cpu;
    Cartridge cartridge;
    byte rom_bank_number, ram_bank_number, mode_flag;
    size_t number_of_rom_banks, number_of_ram_banks;

public:
    Console();
    
    void boot(vector<byte> &data);

    void write(half_word &address, byte value);

    byte read(half_word &address);
};

#endif //DMGB_CONSOLE_HPP