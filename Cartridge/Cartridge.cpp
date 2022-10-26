//
// Created by drake on 27/8/22.
//

#include "Cartridge.hpp"

Cartridge::Cartridge() : number_of_rom_banks(0), number_of_ram_banks(0) {
}

void Cartridge::init(vector<byte> &data) {

    number_of_rom_banks = (1 << data.at(0x148)) * 2;
    rom_banks.resize(number_of_rom_banks, array<byte, rom_block_size>{0});

    number_of_ram_banks = (data.at(0x149) == 0x5) ? 8 : (1 << (2 * data.at(0x149))) / 16;

    ram_banks.resize(number_of_ram_banks, {});

    size_t rom_size = rom_block_size * number_of_rom_banks;
    for (size_t address = 0; address < rom_size; address++) {
        rom_banks.at(address / rom_block_size).at(address % rom_block_size) = data[address];
    }
}

byte Cartridge::get_rom_bank(byte Bank_Number, word &address) {
    return rom_banks[Bank_Number][address];
}

void Cartridge::set_ram_bank(byte Bank_Number, word &address, byte value) {
    ram_banks[Bank_Number][address] = value;
}

byte Cartridge::get_ram_bank(byte Bank_Number, word &address) {
    return ram_banks[Bank_Number][address];
}