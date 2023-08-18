//
// Created by drake on 27/12/22.
//

#ifndef GGBE_MBC0_HPP
#define GGBE_MBC0_HPP

#include "MBC.hpp"

class MBC0 : public MBC {
public:
    byte read_from_rom(word address) override;

    byte read_from_ram(word address) override;

    void write_to_rom(word address, byte value) override;

    void write_to_ram(word address, byte value) override;
};

#endif //GGBE_MBC0_HPP
