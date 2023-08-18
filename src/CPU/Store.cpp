//
// Created by drake on 24/9/22.
//

#include "Cpu.hpp"

void CPU::store_dispatch(int op_id, store::addr_modes addr_mode) {

    switch (addr_mode) {

        case store::addr_modes::SP: { // STORE [u16],SP
            word lo = fetched[1], hi = fetched[2];
            word src_value = get(DReg::sp);
            word dest_address = lo + (hi << 8);

            write(dest_address, src_value & 0xFF);
            write(dest_address + 1, src_value >> 8);
            return;
        }

        case store::addr_modes::IMM: // STORE (HL),u8
        {
            word dest_address = get(DReg::hl);
            byte src_value = fetched.at(1);

            write(dest_address, src_value);
            return;
        }

        case store::addr_modes::REG: // STORE (HL),r8
        {
            word dest_address = get(DReg::hl);
            auto src_index = fetched[0] & 7;
            auto src_reg = static_cast<Reg>(src_index);

            byte src_value = get(src_reg);
            write(dest_address, src_value);
            return;
        }

        case store::addr_modes::MEM_r16: // STORE (r16),A: STORE (BC),A and STORE (DE),A
        {
            DReg dest_reg = static_cast<DReg>(2 * (fetched[0] >> 4));
            word dest_address = get(dest_reg);
            byte src_value = get(Reg::a);

            write(dest_address, src_value);
            return;
        }

        case store::addr_modes::MEM_DI: { // STORE [HL +/-],A
            word dest_address = get(DReg::hl);
            byte src_value = get(Reg::a);

            write(dest_address, src_value);

            if (op_id == store::op::INCREMENT) dest_address++;
            if (op_id == store::op::DECREMENT) dest_address--;

            // INC/DEC HL after store
            set(DReg::hl, dest_address);
            return;
        }

        case store::addr_modes::MEM_IMM: // STORE [u16],A
        {
            word lo = fetched[1], hi = fetched[2];
            byte src_value = get(Reg::a);
            word dest_address = lo + (hi << 8);

            write(dest_address, src_value);
            return;
        }

        case store::addr_modes::HIGH: // STORE_H [C],A and STORE_H [u8],A
        {
            word dest_address = 0xFF00;

            if (op_id == store::op::HIGH_C)
                dest_address += get(Reg::c);
            if (op_id == store::op::HIGH_IMMEDIATE)
                dest_address += fetched[1];
            byte src_value = get(Reg::a);
            write(dest_address, src_value);
            return;
        }
        default:
            std::cout << "Fall through Store\n";
    }

}