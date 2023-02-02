//
// Created by drake on 15/12/22.
//

#include "Mmu.hpp"
#include "MBC0.hpp"
#include "MBC1.hpp"

MMU::MMU(vector<byte> &data)
        : memory_controller{}, vram_segment{}, work_ram_segment{},
          oam_segment{}, io_regs{}, high_ram_segment{}, interrupt_enable{} {

    lyc_written = false;
    reset_timer = false;
    tima_write = false;
    memory_controller = new MBC1();
    memory_controller->init_data(data);
    dma_started = false;
};

MMU::~MMU() {
    delete memory_controller;
}

void MMU::write(word address, byte value) {
    if (address < 0x8000) {
        memory_controller->write_to_rom(address, value);
        return;
    }

    if (address < 0xA000) {
        vram_segment.at(address - 0x8000) = value;
        return;
    }
    if (address < 0xC000) {
        memory_controller->write_to_ram(address, value);
        return;
    }

    if (address < 0xD000) {
        work_ram_segment.at(0).at(address - 0xC000) = value;
        return;
    }

    if (address < 0xE000) {
        work_ram_segment.at(1).at(address - 0xD000) = value;
        return;
    }

    if (address < 0xFE00) return;

    if (address < 0xFEA0) {
        oam_segment.at(address - 0xFE00) = value;
        return;
    }

    if (address < 0xFF00) return;

    if (address < 0xFF80) {

        if (address == div_address)
            reset_timer = true;

        if (address == tima_address)
            tima_write = true;

        if (address == dma_address) {
            dma_started = true;
            dma_transfer(value);
        }

        if (address == lcd_control_address)
            lyc_written = true;

        io_regs.at(address - 0xFF00) = value;

        return;
    }

    if (address < 0xFFFF) {
        high_ram_segment.at(address - 0xFF80) = value;
        return;
    }

    interrupt_enable = value;
}

byte MMU::read(word address) {

    if (address < 0x8000)
        return memory_controller->read_from_rom(address);

    if (address < 0xA000)
        return vram_segment.at(address - 0x8000);

    if (address < 0xC000)
        return memory_controller->read_from_ram(address);

    if (address < 0xD000)
        return work_ram_segment.at(0).at(address - 0xC000);

    if (address < 0xE000)
        return work_ram_segment.at(1).at(address - 0xD000);

    if (address < 0xFE00)
        return 0xFF;

    if (address < 0xFEA0)
        return oam_segment.at(address - 0xFE00);

    if (address < 0xFF00)
        return 0xFF;

    if (address < 0xFF80)
        return io_regs.at(address - 0xFF00);

    if (address < 0xFFFF)
        return high_ram_segment.at(address - 0xFF80);

    return interrupt_enable;
}

void MMU::dma_transfer(byte high_address) {
    word dma_high = high_address << 8;
    for (size_t i = 0; i < 160; i++)
        oam_segment.at(i) = read(dma_high + i);
}