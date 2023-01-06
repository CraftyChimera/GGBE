//
// Created by drake on 15/12/22.
//

#include "Mmu.hpp"
#include "MBC0.hpp"
#include "MBC1.hpp"

MMU::MMU(vector<byte> &data)
        : memory_controller{}, vram_segment{}, work_ram_segment{},
          oam_segment{}, io_regs{}, high_ram_segment{}, interrupt_enable{} {

    reset_timer = false;
    tima_write = false;
    memory_controller = new MBC1();
    memory_controller->init_data(data);
};

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

        if (address == dma_address)
            dma_transfer(value);

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
        return memory_controller->read_from_rom(address);

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
    vector<byte> dma_code = {0x3E, 0x00, 0xE0, 0x46, 0x3E, 0x28, 0x3D, 0x20, 0xFD, 0xC9};
    dma_code.at(1) = high_address;
    for (size_t pos = 0; pos < dma_code.size(); pos++) {
        high_ram_segment.at(pos) = dma_code.at(pos);
    }
}