//
// Created by drake on 15/12/22.
//

#ifndef DMGB_MMU_HPP
#define DMGB_MMU_HPP

#include "../Base/Constants.hpp"
#include "../Cartridge/Cartridge.hpp"

class MMU {
    std::array<byte, memory_map_size> memory;
    bool ram_enabled;
    byte mode_flag;
    byte rom_bank_number, ram_bank_number;
    Cartridge cartridge;

public:
    size_t number_of_rom_banks, number_of_ram_banks;

    MMU();

    void init_data(vector<byte> &data);

    void write(word address, byte value);

    byte read(word address);
};

#endif //DMGB_MMU_HPP
