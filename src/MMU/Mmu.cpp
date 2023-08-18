//
// Created by drake on 15/12/22.
//

#include "Mmu.hpp"
#include "MBC0.hpp"
#include "MBC1.hpp"
#include "MBC2.hpp"
#include "MBC3.hpp"
#include "MBC5.hpp"

MMU::MMU(vector<byte> &data)
        : memory_controller{}, vram_segment{}, work_ram_segment{},
          oam_segment{}, io_regs{}, high_ram_segment{}, interrupt_enable{} {
    lyc_written = false;
    div_write = false;
    tima_write = false;
    tac_write = false;
    tma_write = false;
    dma_started = false;

    load_cartridge_data(data);
}

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
            div_write = true;

        if (address == tima_address)
            tima_write = true;

        if (address == tma_address)
            tma_write = true;

        if (address == tac_address)
            tac_write = true;

        if (address == if_address || address == ie_address) {
            value |= 0xE0;
        }

        if (address == dma_address) {
            dma_started = true;
            dma_transfer(value);
        }

        if (address == lyc_address)
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

void MMU::load_cartridge_data(vector<byte> &data) {
    init_memory_controller(data);
    memory_controller->init_data(data);
}

void MMU::init_memory_controller(vector<byte> &data) {
    auto arg = data.at(0x147);

    switch (arg) {
        case 0x08:
        case 0x09:
        case 0x00:
            memory_controller = new MBC0();
            return;

        case 0x03:
        case 0x02:
        case 0x01:
            memory_controller = new MBC1();
            return;

        case 0x06:
        case 0x05:
            memory_controller = new MBC2();
            return;

        case 0x10:
        case 0x0F:
        case 0x13:
        case 0x12:
        case 0x11:
            memory_controller = new MBC3();
            return;

        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            memory_controller = new MBC5();
            return;

        default:
            throw std::runtime_error("unsupported cartridge type");
    }
}