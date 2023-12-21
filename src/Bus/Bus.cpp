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
    oam_dma_active = false;
    hdma_active = false;
    cgb_dma_source_address = 0;
    cgb_dma_dest_address = 0;
    cgb_dma_length = 0;
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

        if (address == vbk_address) {
            value |= 0xFE;
            vram_bank_number = value & 0x1;
        }

        if (address == svbk_address) {
            wram_bank_number = value & 0x7;
            value |= 0xF8;
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

        if (address == if_address || address == ie_address)
            value |= 0xE0;

        if (address == dma_address)
            start_oam_dma(value);

        if (address == lyc_address)
            lyc_written = true;

        if (address == hdma5_address)
            start_cgb_dma(value);

        io_regs.at(address - 0xFF00) = value;
        return;
    }

    if (address < 0xFFFF) {
        high_ram_segment.at(address - 0xFF80) = value;
        return;
    }

    interrupt_enable = value;
}

byte Bus::read(word address) {
    if (address < 0x8000)
        return memory_controller->read_from_rom(address);

    if (address < 0xA000)
        return vram_segment.at(vram_bank_number).at(address - 0x8000);

    if (address < 0xC000)
        return memory_controller->read_from_ram(address);

    if (address < 0xD000)
        return work_ram_segment.at(0).at(address - 0xC000);

    if (address < 0xE000)
        return work_ram_segment.at(wram_bank_number).at(address - 0xD000);

    if (address < 0xFE00)
        return read(address - 0x2000);

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

void Bus::start_oam_dma(byte high_address) {
    oam_dma_active = true;
    word dma_high = high_address << 8;
    for (size_t i = 0; i < 160; i++)
        oam_segment.at(i) = read(dma_high + i);
}


void Bus::start_cgb_dma(byte hdma5_reg) {
    if (!is_cgb)
        return;

    int source_low = read(hdma2_address) & 0xF0;
    int source_high = read(hdma1_address);
    int dest_low = read(hdma4_address) & 0xF0;
    int dest_high = (read(hdma3_address) & 0x1F) | 0x80;

    cgb_dma_source_address = (source_high << 8) | source_low;
    cgb_dma_dest_address = (dest_high << 8) | dest_low;
    cgb_dma_length = ((hdma5_reg & 0x7F) + 1) * 0x10;
    bool mode = hdma5_reg & (1 << 7);

    if (mode)
        hdma_active = true;
    else {
        if (hdma_active)
            hdma_active = false;
        else
            run_gdma();
    }
}

void Bus::run_gdma() {
    for (int i = 0; i < cgb_dma_length; i++)
        vram_segment.at(vram_bank_number).at(cgb_dma_dest_address++ & 0x1FFF) = read(cgb_dma_source_address++);

    io_regs.at(hdma5_address - 0xFF00) = 0xFF;
}

void Bus::run_hdma_iteration() {
    if (!hdma_active)
        return;

    int length_of_transfer = 0x10;
    for (int i = 0; i < length_of_transfer; i++)
        vram_segment.at(vram_bank_number).at(cgb_dma_dest_address++ & 0x1FFF) = read(
                cgb_dma_source_address++);

    cgb_dma_length -= length_of_transfer;
    hdma_active = (cgb_dma_length != 0);

    if (hdma_active) {
        io_regs.at(hdma5_address - 0xFF00) = ((cgb_dma_length / 0x10) - 1);
        return;
    }

    io_regs.at(hdma1_address - 0xFF00) = 0xFF;
    io_regs.at(hdma2_address - 0xFF00) = 0xFF;
    io_regs.at(hdma3_address - 0xFF00) = 0xFF;
    io_regs.at(hdma4_address - 0xFF00) = 0xFF;
    io_regs.at(hdma5_address - 0xFF00) = 0xFF;
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

byte Bus::read_vram_bank(word address, int bank_no) {
    if (!is_cgb)
        bank_no = 0;
    return vram_segment.at(bank_no).at(address - 0x8000);
}