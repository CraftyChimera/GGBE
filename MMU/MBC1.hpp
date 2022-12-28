//
// Created by drake on 27/12/22.
//

#ifndef DMGB_MBC1_HPP
#define DMGB_MBC1_HPP

#include "../Base/Constants.hpp"
#include "MBC.hpp"

class MBC1 : public MBC {
    bool external_ram_enabled;
    byte mode_flag;
    byte rom_bank_number;
    byte external_ram_bank_number;

public:
    MBC1();

    byte read_from_rom(word address) override;

    byte read_from_ram(word address) override;

    void write_to_rom(word address, byte value) override;

    void write_to_ram(word address, byte value) override;
};

#endif //DMGB_MBC1_HPP
