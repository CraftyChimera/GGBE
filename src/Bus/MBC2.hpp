//
// Created by drake on 11/6/23.
//

#ifndef GGBE_MBC2_HPP
#define GGBE_MBC2_HPP

#include "../Base/Constants.hpp"
#include "MBC.hpp"

auto constexpr internal_ram_size = 0x0200;
using internal_ram_block = array<byte, internal_ram_size>;

class MBC2 : public MBC {
    bool ram_enabled;
    byte rom_bank_number;
    internal_ram_block built_in_ram_segment;

public:
    MBC2();

    byte read_from_rom(word address) override;

    byte read_from_ram(word address) override;

    void write_to_rom(word address, byte value) override;

    void write_to_ram(word address, byte value) override;
};

#endif //GGBE_MBC2_HPP
