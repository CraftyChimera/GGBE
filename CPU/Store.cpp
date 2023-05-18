//
// Created by drake on 24/9/22.
//

#include "Store.hpp"
#include "Cpu.hpp"

void Store::dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, store::addr_modes addr_mode) {

    switch (addr_mode) {

        case store::addr_modes::SP: { // STORE [u16],SP
            word lo = bytes_fetched[1], hi = bytes_fetched[2];
            word src_value = cpu->get(DReg::sp);
            word dest_address = lo + (hi << 8);

            cpu->write(dest_address, src_value & 0xFF);
            cpu->write(dest_address + 1, src_value >> 8);
            return;
        }

        case store::addr_modes::IMM: // STORE (HL),u8
        {
            word dest_address = cpu->get(DReg::hl);
            byte src_value = bytes_fetched.at(1);

            cpu->write(dest_address, src_value);
            return;
        }

        case store::addr_modes::REG: // STORE (HL),r8
        {
            word dest_address = cpu->get(DReg::hl);
            auto src_index = bytes_fetched[0] & 7;
            auto src_reg = static_cast<Reg>(src_index);

            byte src_value = cpu->get(src_reg);
            cpu->write(dest_address, src_value);
            return;
        }

        case store::addr_modes::MEM_r16: // STORE (r16),A: STORE (BC),A and STORE (DE),A
        {
            DReg dest_reg = static_cast<DReg>(2 * (bytes_fetched[0] >> 4));
            word dest_address = cpu->get(dest_reg);
            byte src_value = cpu->get(Reg::a);

            cpu->write(dest_address, src_value);
            return;
        }

        case store::addr_modes::MEM_DI: { // STORE [HL +/-],A
            word dest_address = cpu->get(DReg::hl);
            byte src_value = cpu->get(Reg::a);

            cpu->write(dest_address, src_value);

            if (op_id == store::op::INCREMENT) dest_address++;
            if (op_id == store::op::DECREMENT) dest_address--;

            // INC/DEC HL after store
            cpu->set(DReg::hl, dest_address);
            return;
        }

        case store::addr_modes::MEM_IMM: // STORE [u16],A
        {
            word lo = bytes_fetched[1], hi = bytes_fetched[2];
            byte src_value = cpu->get(Reg::a);
            word dest_address = lo + (hi << 8);

            cpu->write(dest_address, src_value);
            return;
        }

        case store::addr_modes::HIGH: // STORE_H [C],A and STORE_H [u8],A
        {
            word dest_address = 0xFF00;

            if (op_id == store::op::HIGH_C)
                dest_address += cpu->get(Reg::c);
            if (op_id == store::op::HIGH_IMMEDIATE)
                dest_address += bytes_fetched[1];
            byte src_value = cpu->get(Reg::a);
            cpu->write(dest_address, src_value);
            return;
        }
        default:
            std::cout << "Fall through Store\n";
    }

}