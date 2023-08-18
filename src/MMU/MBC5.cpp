//
// Created by drake on 18/8/23.
//

#include "MBC5.hpp"

MBC5::MBC5() : ram_enabled(false), rom_bank_number(1), ram_bank_number(0) {}

byte MBC5::read_from_rom(word address) {
    if (address < 0x4000)
        return rom_segment.at(0).at(address);

    if (address < 0x8000)
        return rom_segment.at(rom_bank_number).at(address - 0x4000);

    return 0xFF;
}

byte MBC5::read_from_ram(word address) {
    if (!ram_enabled)
        return 0xFF;

    return ram_segment.at(ram_bank_number).at(address - 0xA000);
}

void MBC5::write_to_rom(word address, byte value) {
    if (address < 0x2000) {
        if (number_of_ram_banks == 0)
            return;

        if (value == 0xA)
            ram_enabled = true;
        if (value == 0x0)
            ram_enabled = false;
        return;
    }

    if (address < 0x3000) {
        rom_bank_number = rom_bank_number & (1 << 8);
        rom_bank_number = rom_bank_number | value;
        rom_bank_number %= number_of_rom_banks;
        return;
    }

    if (address < 0x4000) {
        rom_bank_number = rom_bank_number & 255;
        rom_bank_number |= (value << 8);
        rom_bank_number %= number_of_rom_banks;
        return;
    }

    if (address < 0x6000) {
        if (value < 0x10) {
            ram_bank_number = value;
            if (number_of_ram_banks > 0)
                ram_bank_number %= number_of_ram_banks;
        }
        return;
    }
}

void MBC5::write_to_ram(word address, byte value) {
    if (!ram_enabled)
        return;

    ram_segment.at(ram_bank_number).at(address - 0xA000) = value;
}