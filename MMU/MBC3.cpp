//
// Created by drake on 18/6/23.
//

#include "MBC3.hpp"

MBC3::MBC3() {
    ram_enabled = false;
    rom_bank_number = 1;
    ram_bank_number = 0;
}

byte MBC3::read_from_rom(word address) {
    if (address < 0x4000)
        return rom_segment.at(0).at(address);

    if (address < 0x8000)
        return rom_segment.at(rom_bank_number).at(address - 0x4000);

    return 0xFF;
}

byte MBC3::read_from_ram(word address) {
    if (!ram_enabled)
        return 0xFF;

    return ram_segment.at(ram_bank_number).at(address - 0xA000);
}

void MBC3::write_to_rom(word address, byte value) {
    if (address < 0x2000) {
        if (number_of_ram_banks == 0)
            return;

        if (value == 0xA)
            ram_enabled = true;
        if (value == 0x0)
            ram_enabled = false;
        return;
    }

    if (address < 0x4000) {
        if (value == 0)
            value = 1;
        rom_bank_number = value;
        rom_bank_number %= number_of_rom_banks;
        return;
    }

    if (address < 0x6000) {
        if (value < 0x04) {
            ram_bank_number = value;
            if (number_of_ram_banks > 0)
                ram_bank_number %= number_of_ram_banks;
        }
        return;
    }
}

void MBC3::write_to_ram(word address, byte value) {
    if (!ram_enabled)
        return;

    ram_segment.at(ram_bank_number).at(address - 0xA000) = value;
}