//
// Created by drake on 15/12/22.
//

#include "Bus.hpp"
#include "MBC0.hpp"
#include "MBC1.hpp"
#include "MBC2.hpp"
#include "MBC3.hpp"
#include "MBC5.hpp"

Bus::Bus(vector<byte> &data)
        : memory_controller{}, oam_segment{}, io_regs{}, high_ram_segment{}, interrupt_enable{} {
    is_cgb = false;
    lcdc_write = false;
    lyc_written = false;
    div_write = false;
    tima_write = false;
    tac_write = false;
    tma_write = false;
    dma_started = false;
    background_palette_written = false;
    obj_palette_written = false;
    vram_bank_number = 0;
    wram_bank_number = 1;

    load_cartridge_data(data);

    if (!is_cgb) {
        vram_segment.resize(1, VRAM());
        work_ram_segment.resize(2, RAM());
    } else {
        vram_segment.resize(2, VRAM());
        work_ram_segment.resize(8, RAM());
    }
}

Bus::~Bus() {
    delete memory_controller;
}

void Bus::write(word address, byte value, bool is_cpu) {
    if (address < 0x8000) {
        memory_controller->write_to_rom(address, value);
        return;
    }

    if (address < 0xA000) {
        vram_segment.at(vram_bank_number).at(address - 0x8000) = value;
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
        work_ram_segment.at(wram_bank_number).at(address - 0xD000) = value;
        return;
    }

    if (address < 0xFE00) return;

    if (address < 0xFEA0) {
        oam_segment.at(address - 0xFE00) = value;
        return;
    }

    if (address < 0xFF00) return;

    if (address < 0xFF80) {
        if (!is_cpu) {
            io_regs.at(address - 0xFF00) = value;
            return;
        }

        if (address == vbk_address)
            value |= 0xFE, vram_bank_number = value & 0x1;

        if (address == svbk_address) {
            wram_bank_number = value & 0x7;
            if (wram_bank_number == 0)
                wram_bank_number = 1;
        }

        if (address == lcd_control_address)
            lcdc_write = true;

        if (address == div_address)
            div_write = true;

        if (address == tima_address)
            tima_write = true;

        if (address == tma_address)
            tma_write = true;

        if (address == tac_address)
            tac_write = true;

        if (address == bcpd_address)
            background_palette_written = true;

        if (address == ocpd_address)
            obj_palette_written = true;

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

byte Bus::read_current_vram_bank(word address) {

    if (address < 0x8000)
        return memory_controller->read_from_rom(address);

    if (address < 0xA000)
        return vram_segment.at(0).at(address - 0x8000);

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

void Bus::dma_transfer(byte high_address) {
    word dma_high = high_address << 8;
    for (size_t i = 0; i < 160; i++)
        oam_segment.at(i) = read_current_vram_bank(dma_high + i);
}

void Bus::load_cartridge_data(vector<byte> &data) {
    byte cgb_data = data.at(0x143);
    std::cout << std::hex << "CGB \t" << (int) data.at(0x143) << "\n";
    if (cgb_data == 0x80 || cgb_data == 0xC0)
        is_cgb = true;

    init_memory_controller(data);
    memory_controller->init_data(data);
}

void Bus::init_memory_controller(vector<byte> &data) {
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

byte Bus::read_other_vram_bank(word address) {
    return vram_segment.at(1 - vram_bank_number).at(address - 0x8000);
}

byte Bus::read_vram_bank(word address, int bank_no) {
    return vram_segment.at(bank_no).at(address - 0x8000);
}
