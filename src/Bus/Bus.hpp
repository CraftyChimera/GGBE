//
// Created by drake on 15/12/22.
//

#ifndef GGBE_BUS_HPP
#define GGBE_BUS_HPP

#include "../Base/Constants.hpp"
#include "MBC.hpp"
// ROM_BANK 00: 0x0000 - 0x3FFF
// ROM_BANK 01-NN: 0x4000 - 0x7FFF
// VRAM: 0x8000 - 0x9FFF
// External RAM: 0xA000 - 0xBFFF
// Work_RAM_BANK 00: 0xC000 - 0xCFFF
// Work_RAM_BANK 01-NN: 0xD000 - 0xDFFF
// ECHO_RAM: 0xE000 - 0xFDFF
// OAM: 0xFE00 - 0xFE9F
// Unused: 0xFEA0 - 0xFEFF
// I/O Registers: 0xFF00 - 0xFF7F
// High_RAM: 0xFF80 - 0xFFFE
// IE: 0xFFFF

inline constexpr auto ram_block_size = 0x1000;
inline constexpr auto vram_size = 0x2000;
inline constexpr auto oam_size = 0x00A0;
inline constexpr auto io_array_size = 0x0080;
inline constexpr auto high_ram_size = 0x007F;

using ram_block = array<byte, ram_block_size>;
using VRAM = array<byte, vram_size>;
using OAM = array<byte, oam_size>;
using IO_array = array<byte, io_array_size>;
using High_RAM = array<byte, high_ram_size>;

class Bus {
    MBC *memory_controller;
    VRAM vram_segment;
    array<ram_block, 2> work_ram_segment;
    OAM oam_segment;
    IO_array io_regs;
    High_RAM high_ram_segment;
    byte interrupt_enable;

public:
    ~Bus();

    bool dma_started;
    
    bool lyc_written;

    bool div_write;

    bool tima_write;

    bool tma_write;

    bool tac_write;

    bool lcdc_write;

    explicit Bus(vector<byte> &data);

    void write(word address, byte value, bool is_cpu = true);

    byte read(word address);

private:
    void load_cartridge_data(vector<byte> &data);

    void init_memory_controller(vector<byte> &data);

    void dma_transfer(byte high_address);
};

#endif //GGBE_BUS_HPP
