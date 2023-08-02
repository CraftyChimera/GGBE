//
// Created by drake on 27/12/22.
//

#include "MBC1.hpp"

MBC1::MBC1() : ram_enabled(false), banking_mode(false), rom_bank_number(1), ram_bank_number(0), zero_bank_number(0) {}

byte MBC1::read_from_rom(word address) {
    if (address < 0x4000)
        return rom_segment.at(zero_bank_number).at(address);

    if (address < 0x8000)
        return rom_segment.at(rom_bank_number).at(address - 0x4000);

    return 0xFF;
}

byte MBC1::read_from_ram(word address) {
    if (!ram_enabled)
        return 0xFF;

    return ram_segment.at(ram_bank_number).at(address - 0xA000);
}

void MBC1::write_to_rom(word address, byte value) {
    if (address < 0x2000) {
        if (number_of_ram_banks == 0)
            return;

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

        rom_bank_number &= 0x1F;
        rom_bank_number |= value << 5;
        zero_bank_number = (banking_mode) ? (value << 5) : 0;
        ram_bank_number = (banking_mode) ? value : 0;

        rom_bank_number %= number_of_rom_banks;
        zero_bank_number %= number_of_rom_banks;
        if (number_of_ram_banks > 0)
            ram_bank_number %= number_of_ram_banks;
        return;
    }

    if (address < 0x8000) {
        banking_mode = (value & 1);

        if (banking_mode) {
            ram_bank_number = (rom_bank_number >> 5) & 0x3;
            zero_bank_number = ram_bank_number << 5;
        } else {
            ram_bank_number = 0;
            zero_bank_number = 0;
        }

        return;
    }
}

void MBC1::write_to_ram(word address, byte value) {
    if (!ram_enabled)
        return;

    ram_segment.at(ram_bank_number).at(address - 0xA000) = value;
}
