//
// Created by drake on 27/12/22.
//

#include "MBC1.hpp"

MBC1::MBC1() {
    external_ram_enabled = false;
    mode_flag = 0;
    rom_bank_number = 0;
    external_ram_bank_number = 0;
}

byte MBC1::read_from_rom(word address) {
    if (address < 0x4000) {
        if (mode_flag == 0)
            return rom_segment.at(0).at(address - 0x0000);
        else {
            byte zero_bank_number = ((rom_bank_number) << 5) & (number_of_rom_banks - 1);
            return rom_segment.at(zero_bank_number).at(address - 0x0000);
        }
    }

    if (address < 0x8000) {
        byte rom_bank_bitmask = (number_of_rom_banks - 1) & 31;
        byte high_bank_number = rom_bank_number & rom_bank_bitmask;
        byte ram_bank_bits = (external_ram_bank_number << 5);

        if (number_of_rom_banks == 64)
            high_bank_number |= ram_bank_bits & (1 << 5);

        if (number_of_rom_banks == 128)
            high_bank_number |= ram_bank_bits & (3 << 5);

        return rom_segment.at(high_bank_number).at(address - 0x4000);
    }

    return 0xFF;
}

byte MBC1::read_from_ram(word address) {
    if (address < 0xA000 || address >= 0xC000) return 0xFF;
    if (!external_ram_enabled) return 0xFF;

    word offset = address - 0xA000;
    if (mode_flag == 0)
        return external_ram_segment.at(0).at(offset);

    if (mode_flag == 1)
        return external_ram_segment.at(external_ram_bank_number).at(offset);

    return 0xFF;
}

void MBC1::write_to_rom(word address, byte value) {
    if (address < 0x2000) {
        external_ram_enabled = ((value & 0xF) == 0xA);
        return;
    }

    if (address < 0x4000) {
        byte bitmask = (number_of_rom_banks - 1) & 31;
        rom_bank_number = (value == 0) ? 1 : value & bitmask;
        return;
    }

    if (address < 0x6000) {
        external_ram_bank_number = value & 3;
        return;
    }

    if (address < 0x8000) {
        mode_flag = address & 1;
        return;
    }
}

void MBC1::write_to_ram(word address, byte value) {
    if (address < 0xA000 || address >= 0xC000) return;
    if (!external_ram_enabled) return;

    word offset = address - 0xA000;
    if (mode_flag == 0)
        external_ram_segment.at(0).at(offset) = value;

    if (mode_flag == 1)
        external_ram_segment.at(external_ram_bank_number).at(offset) = value;
}
