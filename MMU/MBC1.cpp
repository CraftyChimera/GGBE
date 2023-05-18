//
// Created by drake on 27/12/22.
//

#include "MBC1.hpp"

MBC1::MBC1() {
    ram_enabled = false;
    rom_mode = true;
    rom_bank_number = 1;
    ram_bank_number = 0;
}

byte MBC1::read_from_rom(word address) {
    if (address < 0x4000) {
        return rom_segment.at(0).at(address);
    }

    if (address < 0x8000) {
        return rom_segment.at(rom_bank_number).at(address - 0x4000);
    }

    return 0xFF;
}

byte MBC1::read_from_ram(word address) {
    if (!ram_enabled || number_of_ram_banks == 0)
        return 0xFF;

    return ram_segment.at(ram_bank_number).at(address - 0xA000);
}

void MBC1::write_to_rom(word address, byte value) {
    if (address < 0x2000) {
        ram_enabled = ((value & 0xF) == 0xA);
        return;
    }

    if (address < 0x4000) {
        value &= 0x1f;
        if ((value % 0x20) == 0)
            value++;
        rom_bank_number &= 0x60;
        rom_bank_number |= value;
        rom_bank_number %= number_of_rom_banks;
        return;
    }

    if (address < 0x6000) {
        value &= 3;
        if (rom_mode) {
            rom_bank_number &= 0x1F;
            rom_bank_number |= value << 5;
            ram_bank_number = 0;
        } else {
            ram_bank_number = value;
            rom_bank_number &= 0x1F;
        }
        rom_bank_number %= number_of_rom_banks;
        if (ram_enabled)
            ram_bank_number %= number_of_ram_banks;
        return;
    }

    if (address < 0x8000) {
        rom_mode = (value & 1) == 0;
        return;
    }
}

void MBC1::write_to_ram(word address, byte value) {
    if (!ram_enabled) return;

    ram_segment.at(ram_bank_number).at(address - 0xA000) = value;
}
