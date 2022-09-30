//
// Created by drake on 24/9/22.
//

#include "Store.hpp"
#include "Cpu.hpp"

Store::op_args::op_args() {
    src_value = 0;
    destination = 0;
}

void Store::dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {

    if (addr_mode == store::addr_modes::SP) { //LD [u16],SP
        word lo = bytes_fetched[1], hi = bytes_fetched[2], address, value_to_write = cpu->get(DReg::sp);
        address = lo + (hi << 8);

        cpu->write(address, value_to_write & 0xFF);
        cpu->write(address + 1, value_to_write >> 8);
        return;
    }

    if (addr_mode == store::addr_modes::MEM_IMM && op_id != store::op::NOTHING) { //STORE_H [C],A and STORE_H [u8],A
        if (op_id == store::op::HIGH_C) //STORE_H [C],A
            bytes_fetched.emplace_back(cpu->get(Reg::c));
        bytes_fetched.emplace_back(0xFF);
    }

    auto args = Store::get_args(cpu, bytes_fetched, addr_mode);

    if (addr_mode == store::addr_modes::MEM_DI) { //STORE [HL I/D],A
        auto value = cpu->get(DReg::hl);
        value += ((op_id == store::op::INCREMENT) * (1) + (op_id == store::op::DECREMENT) * (-1));
        cpu->set(DReg::hl, value);
    }

    cpu->write(args.destination, args.src_value);
}


Store::op_args Store::get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Store::op_args result;
    switch (addressing_mode) {
        case store::addr_modes::IMM: //STORE [HL],u8
        {
            result.src_value = bytes_fetched[1];
            result.destination = cpu->get(DReg::hl);
            break;
        }
        case store::addr_modes::REG ://STORE [HL].r8
        {
            Reg source_reg = static_cast<Reg>(bytes_fetched[0] & 0x7);
            result.src_value = cpu->get(source_reg);
            result.destination = cpu->get(DReg::hl);
            break;
        }
        case store::addr_modes::MEM_r16: //STORE [r16],A
        {
            DReg dest_reg = static_cast<DReg>(2 * (bytes_fetched[0] >> 4));
            result.src_value = cpu->get(Reg::a);
            result.destination = cpu->get(dest_reg);
            break;
        }
        case store::addr_modes::MEM_DI: //STORE [HL I/D],A
        {
            result.src_value = cpu->get(Reg::a);
            result.destination = cpu->get(DReg::hl);
            break;
        }
        case store::addr_modes::MEM_IMM: //STORE [u16],A
        {
            word address = bytes_fetched[1], hi = bytes_fetched[2];
            address += (hi << 8);
            result.src_value = cpu->get(Reg::a);
            result.destination = address;
            break;
        }
        default:
            break;
    }
    return result;
}