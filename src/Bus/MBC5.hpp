//
// Created by drake on 18/8/23.
//

#ifndef GGBE_MBC5_HPP
#define GGBE_MBC5_HPP

#include "../Base/Constants.hpp"
#include "MBC.hpp"

class MBC5 : public MBC {
    bool ram_enabled;
    word rom_bank_number;
    byte ram_bank_number;

public:
    MBC5();

    byte read_from_rom(word address) override;

    byte read_from_ram(word address) override;

    void write_to_rom(word address, byte value) override;

    void write_to_ram(word address, byte value) override;
};

#endif //GGBE_MBC5_HPP
