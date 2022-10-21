//
// Created by drake on 17/9/22.
//

#include "Load.hpp"
#include "Cpu.hpp"

Load::op_args::op_args() {
    src_value = 0;
    destination = Reg::a;
}

void Load::dispatch([[maybe_unused]] vector<Flag_Status> &flags, CPU *cpu, int op_id, vector<byte> &bytes_fetched,
                    int addr_mode) {

    if (addr_mode == load::addr_modes::IMM_16) { //LD r16,u16
        int bitmask = (bytes_fetched[0] >> 4);
        DReg reg_index = static_cast<DReg>(2 * bitmask);
        word lo = bytes_fetched[1], hi = bytes_fetched[2];
        cpu->set(reg_index, lo + (hi << 8));
        return;
    }

    if (addr_mode == load::addr_modes::REL_16) { //LD HL,SP+i8
        auto offset = static_cast<s_byte>(bytes_fetched[1]);
        word src_value = cpu->get(DReg::sp);
        cpu->set(DReg::hl, src_value + offset);
        return;
    }
    if (addr_mode == load::addr_modes::SP) { //LD SP,HL
        word src_value = cpu->get(DReg::hl);
        cpu->set(DReg::sp, src_value);
        return;
    }

    if (addr_mode == load::addr_modes::MEM_IMM && op_id != load::op::NOTHING) { //LDH A,[C] and LDH A,[u8]
        if (op_id == load::op::HIGH_C) //LDH A,[C]
            bytes_fetched.emplace_back(cpu->get(Reg::c));
        bytes_fetched.emplace_back(0xFF);
    }

    auto args = Load::get_args(cpu, bytes_fetched, addr_mode);

    if (addr_mode == load::addr_modes::MEM_DI) { //LD A,[HL+]/[HL-]
        auto value = cpu->get(DReg::hl);
        value += ((op_id == load::op::INCREMENT) * (1) + (op_id == load::op::DECREMENT) * (-1));
        cpu->set(DReg::hl, value);
    }

    cpu->set(args.destination, args.src_value);
}

Load::op_args Load::get_args(CPU *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Load::op_args result;
    switch (addressing_mode) {
        case load::addr_modes::IMM : // Load r8,u8
        {
            byte reg_calc = 2 * (bytes_fetched[0] >> 4) + ((bytes_fetched[0] & 0x8) > 0);
            result.destination = static_cast<Reg>(reg_calc);
            result.src_value = bytes_fetched[1];
            break;
        }
        case load::addr_modes::REG : //Load r8,r8
        {
            Reg source_reg = static_cast<Reg>(bytes_fetched[0] & 0x7);
            Reg dest_reg = static_cast<Reg>((bytes_fetched[0] >> 3) & 0x7);
            result.src_value = cpu->get(source_reg);
            result.destination = dest_reg;
            break;
        }
        case load::addr_modes::MEM_HL ://Load r8,[HL]
        {
            result.src_value = cpu->read(cpu->get(DReg::hl));
            result.destination = static_cast<Reg>((bytes_fetched[0] >> 3) & 0x7);
            break;
        }
        case load::addr_modes::MEM_r16 ://Load A,[r16]
        {
            DReg source_reg = static_cast<DReg>(2 * (bytes_fetched[0] >> 4));
            result.src_value = cpu->get(source_reg);
            result.destination = Reg::a;
            break;
        }
        case load::addr_modes::MEM_DI ://Load A,[HL+] ,Load A,[HL-]
        {
            result.src_value = cpu->read(cpu->get(DReg::hl));
            result.destination = Reg::a;
            break;
        }
        case load::addr_modes::MEM_IMM ://Load A,[u16]
        {
            word lo = bytes_fetched[1], hi = bytes_fetched[2];
            result.src_value = cpu->read(lo + (hi << 8));
            result.destination = Reg::a;
            break;
        }
        default:
            break;
    }
    return result;
}