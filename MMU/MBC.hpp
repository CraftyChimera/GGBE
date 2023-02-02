//
// Created by drake on 27/12/22.
//

#ifndef DMGB_MBC_HPP
#define DMGB_MBC_HPP

#include "../Base/Constants.hpp"
#include <array>

using std::array;

inline constexpr auto rom_block_size = 0x4000;
inline constexpr auto external_ram_size = 0x2000;

using rom_block = array<byte, rom_block_size>;
using external_ram_block = array<byte, external_ram_size>;

class MBC {
public:
    size_t number_of_rom_banks{};
    size_t number_of_ram_banks{};

    vector<rom_block> rom_segment;
    vector<external_ram_block> ram_segment;

    MBC() = default;

    virtual ~MBC() = default;

    void init_data(vector<byte> &data);

    virtual byte read_from_rom(word address) = 0;

    virtual byte read_from_ram(word address) = 0;

    virtual void write_to_rom(word address, byte value) = 0;

    virtual void write_to_ram(word address, byte value) = 0;
};

#endif //DMGB_MBC_HPP