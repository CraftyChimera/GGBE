//
// Created by drake on 27/12/22.
//

#include "MBC.hpp"

void MBC::init_data(vector<byte> &data) {
    byte ram_identifier = data.at(0x149);
    constexpr int number_of_ram_banks_map[6] = {0, -1, 1, 4, 16, 8};
    number_of_ram_banks = number_of_ram_banks_map[ram_identifier];
    ram_segment.resize(number_of_ram_banks, {});

    // Total rom size = 32 KiB * (1 << data.at(0x148)), one block = 16 KiB
    byte rom_identifier = data.at(0x0148);
    number_of_rom_banks = (2 << rom_identifier);
    rom_segment.resize(number_of_rom_banks, {});
    size_t rom_size = rom_block_size * number_of_rom_banks;
    for (size_t address = 0; address < rom_size; address++) {
        rom_segment.at(address / rom_block_size).at(address % rom_block_size) = data.at(address);
    }

    std::cout << std::dec << "Rom:\t" << number_of_rom_banks << "\n";
    std::cout << "MBC: \t" << (int) data.at(0x147) << "\n";
}
