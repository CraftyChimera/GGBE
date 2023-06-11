//
// Created by drake on 17/9/22.
//

#include "Load.hpp"
#include "Cpu.hpp"

void Load::dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, load::addr_modes addr_mode) {
    switch (addr_mode) {

        case load::addr_modes::SP: //LD SP,HL
        {
            DReg src_reg = DReg::hl;
            DReg dest_reg = DReg::sp;

            word src_value = cpu->get(src_reg);
            cpu->set(dest_reg, src_value);
            cpu->tick_components();
            return;
        }

        case load::addr_modes::REL_16: //LD HL,SP+i8
        {
            auto signed_offset = get_signed_offset(bytes_fetched[1]);

            DReg dest_reg = DReg::hl;
            auto src_value = cpu->get(DReg::sp);

            bool z_bit = false;
            bool n_bit = false;
            bool h_bit = (src_value & 0xF) + (signed_offset & 0xF) > 0xF;
            bool c_bit = (src_value & 0xFF) + (signed_offset & 0xFF) > 0xFF;

            auto flags = batch_fill({z_bit, n_bit, h_bit, c_bit});

            src_value += signed_offset;

            cpu->set_flags();
            cpu->set(dest_reg, src_value);
            cpu->tick_components();
            return;
        }

        case load::addr_modes::IMM_16:  //LD r16,u16
        {
            int bitmask = bytes_fetched[0] >> 4;
            DReg reg_index = static_cast<DReg>(2 * bitmask);
            word lo = bytes_fetched.at(1), hi = bytes_fetched.at(2);

            //Little Endian machine
            word src_value = lo + (hi << 8);
            cpu->set(reg_index, src_value);
            return;
        }

        case load::addr_modes::IMM: //LD r8,u8
        {
            auto src_value = bytes_fetched[1];
            auto row = bytes_fetched[0] >> 4;

            auto column = -1;
            auto reg_mask = bytes_fetched[0] & 0xF;
            if (reg_mask == 0x6) column = 0;
            if (reg_mask == 0xE) column = 1;

            byte reg_calc = 2 * row + column;
            Reg dest_reg = static_cast<Reg>(reg_calc);

            cpu->set(dest_reg, src_value);
            return;
        }

        case load::addr_modes::REG : //Load r8,r8
        {
            Reg src_reg = static_cast<Reg>(bytes_fetched[0] & 0x7);
            Reg dest_reg = static_cast<Reg>((bytes_fetched[0] >> 3) & 0x7);

            auto src_value = cpu->get(src_reg);
            cpu->set(dest_reg, src_value);
            return;
        }

        case load::addr_modes::MEM_HL: //Load r8,(HL)
        {
            word src_address = cpu->get(DReg::hl);
            Reg dest_reg = static_cast<Reg>((bytes_fetched[0] >> 3) & 0x7);

            auto src_value = cpu->read(src_address);
            cpu->set(dest_reg, src_value);
            return;
        }

        case load::addr_modes::MEM_r16: //Load A,(BC) and LOAD A,(DE)
        {
            auto src_index = 2 * (bytes_fetched[0] >> 4);
            DReg src_reg = static_cast<DReg>(src_index);
            word src_address = cpu->get(src_reg);
            Reg dest_reg = Reg::a;

            byte src_value = cpu->read(src_address);
            cpu->set(dest_reg, src_value);
            return;
        }

        case load::addr_modes::MEM_DI: //LD A,[HL+/-]
        {
            word address = cpu->get(DReg::hl);
            Reg dest_reg = Reg::a;
            byte src_value = cpu->read(address);

            if (op_id == load::op::INCREMENT) address++;
            if (op_id == load::op::DECREMENT) address--;

            cpu->set(DReg::hl, address);
            cpu->set(dest_reg, src_value);
            return;
        }

        case load::addr_modes::MEM_IMM: //LOAD A,[u16]
        {
            byte lo = bytes_fetched.at(1), hi = bytes_fetched.at(2);
            word address = lo + (hi << 8);
            Reg dest_reg = Reg::a;

            byte src_value = cpu->read(address);
            cpu->set(dest_reg, src_value);
            return;
        }

        case load::addr_modes::HIGH: //LDH A,[C] LDH A,[u8]
        {
            word address = 0xFF00;
            Reg dest_reg = Reg::a;

            if (op_id == load::op::HIGH_C)
                address += cpu->get(Reg::c);
            if (op_id == load::op::HIGH_IMMEDIATE)
                address += bytes_fetched.at(1);

            byte src_value = cpu->read(address);
            cpu->set(dest_reg, src_value);
            return;
        }

        default: {
            std::cout << "Fall through Load\n";
            return;
        }
    }

}