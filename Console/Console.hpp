//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "../Base/Constants.hpp"
#include "../CPU/Cpu.hpp"
#include "../GPU/Gpu.hpp"
#include "../Cartridge/Cartridge.hpp"

class GPU;

class Cartridge;

class CPU;

class Console {
    CPU cpu;
    std::array<byte, memory_map_size> memory;

    bool ram_enabled;
    Cartridge cartridge;
    GPU renderer;

    byte rom_bank_number, ram_bank_number, mode_flag;
    size_t number_of_rom_banks, number_of_ram_banks;

public:
    Console();

    void run(vector<byte> &data);

    void write(word address, byte value);

    byte read(word address);

private:
    void init(vector<byte> &data);

    void loop();

    void boot_rom();
};

#endif //DMGB_CONSOLE_HPP