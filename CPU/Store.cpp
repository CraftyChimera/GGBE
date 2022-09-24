//
// Created by drake on 24/9/22.
//

#include "Store.hpp"
#include "Cpu.hpp"

Store::op_args::op_args() {
    src_value = 0;
    destination = 0;
}

void dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {
    if (addr_mode == 4 && op_id != 0) {//STORE_H [C],A and STORE_H [u8],A
        if (op_id == 1) //STORE_H [C],A
            bytes_fetched.emplace_back(cpu->get(Reg::c));
        bytes_fetched.emplace_back(0xFF);
    }

    auto args = Store::get_args(cpu, bytes_fetched, addr_mode);

    if (addr_mode == 3) {
        auto value = cpu->get(DReg::hl);
        value += ((op_id == 1) * (1) + (op_id == 2) * (-1));
        cpu->set(DReg::hl, value);
    }

    cpu->write(args.destination, args.src_value);
}


Store::op_args Store::get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Store::op_args result;
    switch (addressing_mode) {
        case 0: //STORE [HL],u8
        {
            result.src_value = bytes_fetched[1];
            result.destination = cpu->get(DReg::hl);
            break;
        }
        case 1://STORE [HL].r8
        {
            Reg source_reg = static_cast<Reg>(bytes_fetched[0] & 0x7);
            result.src_value = cpu->get(source_reg);
            result.destination = cpu->get(DReg::hl);
            break;
        }
        case 2: //STORE [r16],A
        {
            DReg dest_reg = static_cast<DReg>(2 * (bytes_fetched[0] >> 4));
            result.src_value = cpu->get(Reg::a);
            result.destination = cpu->get(dest_reg);
            break;
        }
        case 3: //STORE [HL I/D],A
        {
            result.src_value = cpu->get(Reg::a);
            result.destination = cpu->get(DReg::hl);
            break;
        }
        case 4: //STORE [u16],A
        {
            word address = bytes_fetched[1];
            address += bytes_fetched[2] << 4;
            result.src_value = cpu->get(Reg::a);
            result.destination = address;
            break;
        }
        default:
            break;
    }
}