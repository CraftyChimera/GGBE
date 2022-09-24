//
// Created by drake on 17/9/22.
//

#include "Load.hpp"
#include "Cpu.hpp"

Load::op_args::op_args() {
    src_value = 0;
    destination = Reg::a;
}

void Load::dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {

    if (addr_mode == -1) {
        int bitmask = (bytes_fetched[0] >> 4);
        DReg reg_index = static_cast<DReg>(2 * bitmask);
        word value = bytes_fetched[1];
        value += bytes_fetched[2] << 4;
        cpu->set(reg_index, value);
        return;
    }

    if (addr_mode == 5 && op_id != 0) {//LDH A,[C] and LDH A,[u8]
        if (op_id == 1) //LDH A,[C]
            bytes_fetched.emplace_back(cpu->get(Reg::c));
        bytes_fetched.emplace_back(0xFF);
    }

    auto args = Load::get_args(cpu, bytes_fetched, addr_mode);

    if (addr_mode == 4) {
        auto value = cpu->get(DReg::hl);
        value += ((op_id == 1) * (1) + (op_id == 2) * (-1));
        cpu->set(DReg::hl, value);
    }

    cpu->set(args.destination, args.src_value);
}

Load::op_args get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Load::op_args result;
    switch (addressing_mode) {
        case 0: // Load r8,u8
        {
            byte reg_calc = 2 * (bytes_fetched[0] >> 4) + ((bytes_fetched[0] & 0x8) > 0);
            result.destination = static_cast<Reg>(reg_calc);
            result.src_value = bytes_fetched[1];
            break;
        }
        case 1: //Load r8,r8
        {
            Reg source_reg = static_cast<Reg>(bytes_fetched[0] & 0x7);
            Reg dest_reg = static_cast<Reg>((bytes_fetched[0] >> 3) & 0x7);
            result.src_value = cpu->get(source_reg);
            result.destination = dest_reg;
            break;
        }
        case 2://Load r8,[HL]
        {
            result.src_value = cpu->read(cpu->get(DReg::hl));
            result.destination = static_cast<Reg>((bytes_fetched[0] >> 3) & 0x7);
            break;
        }
        case 3://Load A,[r16]
        {
            DReg source_reg = static_cast<DReg>(2 * (bytes_fetched[0] >> 4));
            result.src_value = cpu->get(source_reg);
            result.destination = Reg::a;
            break;
        }
        case 4://Load A,[HL+] ,Load A,[HL-]
        {
            result.src_value = cpu->read(cpu->get(DReg::hl));
            result.destination = Reg::a;
            break;
        }
        case 5://Load A,[u16]
        {
            word address = bytes_fetched[1];
            address += bytes_fetched[2] << 4;
            result.src_value = cpu->read(address);
            result.destination = Reg::a;
            break;
        }
        default:
            break;
    }
    return result;
}