//
// Created by drake on 27/12/22.
//

#include "MBC0.hpp"

byte MBC0::read_from_rom(word address) {

    if (address < 0x4000) { return rom_segment.at(0).at(address - 0x0000); }
    if (address < 0x8000) { return rom_segment.at(1).at(address - 0x4000); }
    return 0xFF;
}

byte MBC0::read_from_ram(word address) {
    std::cout << "Illegal ram_read\t" << address << "\n";
    return 0xFF;
}

void MBC0::write_to_rom(word address, byte value) {
    std::cout << std::hex << "Illegal rom write\t" << address << " " << (int) value << "\n";
}

void MBC0::write_to_ram(word address, byte value) {
    std::cout << std::hex << "Illegal ram write\t" << address << " " << (int) value << "\n";
}