//
// Created by drake on 24/9/22.
//

#include "Jump_and_Stack.hpp"
#include "Cpu.hpp"

const std::function<void(CPU *, Jump::op_args &)> Jump::op_codes[9] = {Jump::JP, Jump::JPC, Jump::CALL, Jump::CALLC,
                                                                       Jump::RET, Jump::RETC, Jump::RETI, Jump::PUSH,
                                                                       Jump::POP};


Jump::op_args::op_args() {
    jump_address = 0;
    condition = -1;
}

void Jump::dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, jump_stack::addr_modes addr_mode) {
    if (bytes_fetched[0] == 0xE9) {
        word address = cpu->get(DReg::hl);
        cpu->set_pc(address, false);
        return;
    }
    auto args = Jump::get_args(cpu, bytes_fetched, addr_mode);
    Jump::op_codes[op_id](cpu, args);
}


bool checkCondition(byte F, int cond_id) {
    bool C = F & (1 << Flag::c);
    bool Z = F & (1 << Flag::z);
    switch (cond_id) {
        case 0:
            return !Z;
        case 1:
            return Z;
        case 2:
            return !C;
        case 3:
            return C;
        default:
            return false;
    }
}

void Jump::PUSH(CPU *cpu, Jump::op_args &args) {
    DReg reg = static_cast<DReg>(args.condition);

    if (reg == DReg::sp)
        reg = DReg::af;

    word next = cpu->get(reg);
    byte hi = next >> 8, lo = next & 0xFF;
    cpu->push(hi);
    cpu->push(lo);
    cpu->tick_components();
}

void Jump::POP(CPU *cpu, Jump::op_args &args) {
    DReg reg = static_cast<DReg>(args.condition);

    if (reg == DReg::sp)
        reg = DReg::af;

    byte lo = cpu->pop();
    byte hi = cpu->pop();

    word value = lo + (hi << 8);

    if (reg == DReg::af)
        value &= 0xFFF0;

    cpu->set(reg, value);
}

void Jump::JP(CPU *cpu, Jump::op_args &args) {
    cpu->set_pc(args.jump_address);
}

void Jump::JPC(CPU *cpu, Jump::op_args &args) {
    if (checkCondition(cpu->get(Reg::f), args.condition)) {
        Jump::JP(cpu, args);
    }
}

void Jump::CALL(CPU *cpu, Jump::op_args &args) { //RST
    word next = cpu->get(DReg::pc);
    byte hi = next >> 8, lo = next & 0xFF;
    cpu->push(hi);
    cpu->push(lo);
    Jump::JP(cpu, args);
}

void Jump::CALLC(CPU *cpu, Jump::op_args &args) {
    if (checkCondition(cpu->get(Reg::f), args.condition)) {
        Jump::CALL(cpu, args);
    }
}

void Jump::RET(CPU *cpu, [[maybe_unused]] Jump::op_args &args) {
    word lo, hi;
    lo = cpu->pop();
    hi = cpu->pop();
    cpu->set_pc(lo + (hi << 8));
}

void Jump::RETC(CPU *cpu, Jump::op_args &args) {
    cpu->tick_components();
    if (checkCondition(cpu->get(Reg::f), args.condition)) {
        Jump::RET(cpu, args);
    }
}

void Jump::RETI(CPU *cpu, Jump::op_args &args) {
    Jump::RET(cpu, args);
    cpu->interrupt_buffer = 1;
}

Jump::op_args Jump::get_args(CPU *cpu, vector<byte> &bytes_fetched, jump_stack::addr_modes addressing_mode) {
    Jump::op_args result;
    switch (addressing_mode) {
        case jump_stack::addr_modes::PUSH_POP :// PUSH/POP r16
        {
            byte opc = bytes_fetched[0];
            result.condition = 2 * ((opc >> 4) & 0x3);
            break;
        }
        case jump_stack::addr_modes::NOTHING :
        case jump_stack::addr_modes::MEM : //JP HL
        {
            byte opc = bytes_fetched[0];
            result.condition = 2 * ((opc & 0x10) > 0) + ((opc & 0x08) > 0);
            result.jump_address = cpu->get(DReg::hl);
            break;
        }
        case jump_stack::addr_modes::IMM ://JP u16
        {
            word address = bytes_fetched[1], hi = bytes_fetched[2];
            address += (hi << 8);
            byte opc = bytes_fetched[0];
            result.condition = 2 * ((opc & 0x10) > 0) + ((opc & 0x08) > 0);
            result.jump_address = address;
            break;
        }
        case jump_stack::addr_modes::REL ://JR e8
        {
            auto offset = static_cast<s_byte>(bytes_fetched.at(1));
            byte opc = bytes_fetched[0];

            result.condition = 2 * ((opc & 0x10) > 0) + ((opc & 0x08) > 0);
            result.jump_address = cpu->get(DReg::pc) + offset;

            if (opc == 0x30)
                result.condition = 2;
            break;
        }
        case jump_stack::addr_modes::RST ://RSTs
        {
            byte opc = bytes_fetched[0];
            result.jump_address = opc - 0xC7;
            break;
        }
        default:
            break;
    }
    return result;
}