//
// Created by drake on 27/8/22.
//

#include "Cartridge.hpp"

Cartridge::Cartridge() = default;

void Cartridge::init(vector<byte> &data) {

    auto no_of_rom_banks = (1 << data[0x148]) * 2;
    rom_banks.resize(no_of_rom_banks, array<byte, rom_block_size>{0});

    auto no_of_ram_banks = (1 << (2 * data[0x149])) / 16;
    ram_banks.resize(no_of_ram_banks, array<byte, ram_block_size>{0});

    size_t rom_size = rom_block_size * no_of_rom_banks;
    for (size_t address = 0; address < rom_size; address++) {
        rom_banks[address / rom_block_size][address % rom_block_size] = data[address];
    }
}