//
// Created by drake on 18/6/23.
//

#ifndef GGBE_MBC3_HPP
#define GGBE_MBC3_HPP

#include "../Base/Constants.hpp"
#include "MBC.hpp"

class MBC3 : public MBC {
    bool ram_enabled;
    byte rom_bank_number;
    byte ram_bank_number;

public:
    MBC3();

    byte read_from_rom(word address) override;

    byte read_from_ram(word address) override;

    void write_to_rom(word address, byte value) override;

    void write_to_ram(word address, byte value) override;
};

#endif //GGBE_MBC3_HPP
