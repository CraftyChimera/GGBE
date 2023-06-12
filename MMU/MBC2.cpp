//
// Created by drake on 11/6/23.
//

#include "MBC2.hpp"

MBC2::MBC2() {
    ram_enabled = false;
    rom_bank_number = 1;
    built_in_ram_segment = {};
}

byte MBC2::read_from_rom(word address) {
    if (address < 0x4000) {
        return rom_segment.at(0).at(address);
    }

    if (address < 0x8000) {
        return rom_segment.at(rom_bank_number).at(address - 0x4000);
    }
    std::cout << "Illegal rom read >= 0x8000 \n";
    return 0xFF;
}

byte MBC2::read_from_ram(word address) {
    if (!ram_enabled)
        return 0xFF;

    if (address < 0xA000) {
        std::cout << "Illegal ram read < 0xA000 \n";
        return 0xFF;
    }

    if (address < 0xA200) {
        return built_in_ram_segment.at(address - 0xA000);
    }

    if (address < 0xC000) {
        word effective_address = (address - 0xA000) & 0x01FF;
        return built_in_ram_segment.at(effective_address);
    }

    std::cout << "Illegal ram read >= 0xC000 \n";
    return 0xFF;
}

void MBC2::write_to_rom(word address, byte value) {
    if (address >= 0x4000) {
        std::cout << "Illegal rom write >=0x4000 \n";
        return;
    }

    bool control_rom_bank = (address >> 8) & 1;
    if (control_rom_bank) {
        rom_bank_number = value & 0x0F;
        if (rom_bank_number == 0)
            rom_bank_number = 1;
        rom_bank_number %= number_of_rom_banks;
    } else {
        ram_enabled = ((value & 0xF) == 0xA);
    }
};

void MBC2::write_to_ram(word address, byte value) {
    if (!ram_enabled)
        return;
    value &= 0x0F;
    value += 0xF0;
    built_in_ram_segment.at(address - 0xA000) = value;
}