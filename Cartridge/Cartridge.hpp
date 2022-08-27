//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CARTRIDGE_HPP
#define DMGB_CARTRIDGE_HPP
#endif //DMGB_CARTRIDGE_HPP

#include "../Base/Common.hpp"

constexpr auto rom_block_size = 0x4000;
constexpr auto ram_block_size = 0x2000;

class Cartridge {
    vector<array<byte, rom_block_size>> rom_banks;
    vector<array<byte, ram_block_size>> ram_banks;

public:
    Cartridge();

    void init(vector<byte> &data);
};