//
// Created by drake on 27/8/22.
//

#include "Cartridge.hpp"

Cartridge::Cartridge() : rom_banks({}), ram_banks({}), number_of_rom_banks(0), number_of_ram_banks(0) {
}


void Cartridge::init(vector<byte> &data) {
    constexpr int number_of_ram_banks_map[6] = {0, -1, 1, 4, 16, 8};
    const int ram_bank_identifier = data.at(0x149);
    number_of_ram_banks = number_of_ram_banks_map[ram_bank_identifier];
    ram_banks.resize(number_of_ram_banks, {});

    // Total rom size = 32 KiB * (1 << data.at(0x148)), one block = 16 KiB
    const int rom_bank_identifier = data.at(0x148);
    number_of_rom_banks = (2 << rom_bank_identifier);
    rom_banks.resize(number_of_rom_banks, {});

    size_t rom_size = rom_block_size * number_of_rom_banks;
    for (size_t address = 0; address < rom_size; address++) {
        rom_banks.at(address / rom_block_size).at(address % rom_block_size) = data.at(address);
    }
}

byte Cartridge::get_rom_bank(byte Bank_Number, word &address) {
    return rom_banks.at(Bank_Number).at(address);
}

void Cartridge::set_ram_bank(byte Bank_Number, word &address, byte value) {
    ram_banks.at(Bank_Number).at(address) = value;
}

byte Cartridge::get_ram_bank(byte Bank_Number, word &address) {
    return ram_banks[Bank_Number][address];
}