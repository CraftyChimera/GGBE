//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CARTRIDGE_HPP
#define DMGB_CARTRIDGE_HPP

#include "../Base/Common.hpp"

class Cartridge {
    vector<array<byte, rom_block_size>> rom_banks;
    vector<array<byte, ram_block_size>> ram_banks;
public:
    size_t number_of_rom_banks;
    size_t number_of_ram_banks;

public:
    Cartridge();

    void init(vector<byte> &data);

    byte get_rom_bank(byte Bank_Number, word &address);

    void set_ram_bank(byte Bank_Number, word &address, byte value);

    byte get_ram_bank(byte Bank_Number, word &address);
};

#endif //DMGB_CARTRIDGE_HPP