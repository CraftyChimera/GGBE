//
// Created by drake on 24/9/22.
//

#include "Cpu.hpp"

CPU::jump_stack_op_args::jump_stack_op_args() : jump_address(0), condition(-1) {}

void CPU::jump_stack_dispatch(int op_id, jump_stack::addr_modes addr_mode) {
    if (jump_stack_check_and_deal_with_edge_cases(op_id))
        return;

    auto args = jump_stack_get_args(addr_mode);
    jump_execute_stack_set_result(op_id, args);
}

bool CPU::jump_stack_check_and_deal_with_edge_cases(int op_id) {
    if (op_id == 0xE9) {
        word address = get(DReg::hl);
        set_pc(address, false);
        return true;
    }
    return false;
}

void CPU::jump_execute_stack_set_result(int op_id, jump_stack_op_args &args) {
    switch (op_id) {
        case jump_stack::op::JP:
            JP(args);
            return;
        case jump_stack::op::JPC:
            JPC(args);
            return;
        case jump_stack::op::CALL:
            CALL(args);
            return;
        case jump_stack::op::CALLC:
            CALLC(args);
            return;
        case jump_stack::op::RET:
            RET(args);
            return;
        case jump_stack::op::RETC:
            RETC(args);
            return;
        case jump_stack::op::RETI:
            RETI(args);
            return;
        case jump_stack::op::POP:
            POP(args);
            return;
        case jump_stack::op::PUSH:
            PUSH(args);
            return;
        default:
            return;
    }
}

bool checkCondition(byte F, int condition_id) {
    bool C = F & (1 << Flag::c);
    bool Z = F & (1 << Flag::z);
    switch (condition_id) {
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

void CPU::PUSH(jump_stack_op_args &args) {
    DReg reg = static_cast<DReg>(args.condition);

    if (reg == DReg::sp)
        reg = DReg::af;

    word next = get(reg);
    byte hi = next >> 8;
    byte lo = next & 0xFF;
    push(hi);
    push(lo);
    tick_components();
}

void CPU::POP(jump_stack_op_args &args) {
    DReg reg = static_cast<DReg>(args.condition);

    if (reg == DReg::sp)
        reg = DReg::af;

    byte lo = pop();
    byte hi = pop();

    word value = lo + (hi << 8);

    if (reg == DReg::af)
        value &= 0xFFF0;

    set(reg, value);
}

void CPU::JP(jump_stack_op_args &args) {
    set_pc(args.jump_address);
}

void CPU::JPC(jump_stack_op_args &args) {
    if (checkCondition(get(Reg::f), args.condition)) {
        CPU::JP(args);
    }
}

void CPU::CALL(jump_stack_op_args &args) { //RST
    word new_pc = get(DReg::pc);
    byte hi = new_pc >> 8, lo = new_pc & 0xFF;
    push(hi);
    push(lo);
    JP(args);
}

void CPU::CALLC(jump_stack_op_args &args) {
    if (checkCondition(get(Reg::f), args.condition)) {
        CALL(args);
    }
}

void CPU::RET([[maybe_unused]]jump_stack_op_args &args) {
    auto lo = pop();
    auto hi = pop();
    set_pc(lo + (hi << 8));
}

void CPU::RETC(jump_stack_op_args &args) {
    tick_components();
    if (checkCondition(get(Reg::f), args.condition)) {
        RET(args);
    }
}

void CPU::RETI(jump_stack_op_args &args) {
    RET(args);
    interrupt_buffer = 1;
}

CPU::jump_stack_op_args CPU::jump_stack_get_args(jump_stack::addr_modes addressing_mode) {
    jump_stack_op_args result;
    switch (addressing_mode) {
        case jump_stack::addr_modes::PUSH_POP :// PUSH/POP r16
        {
            byte opc = fetched[0];
            result.condition = 2 * ((opc >> 4) & 0x3);
            break;
        }
        case jump_stack::addr_modes::NOTHING :
        case jump_stack::addr_modes::MEM : //JP HL
        {
            byte opc = fetched[0];
            result.condition = 2 * ((opc & 0x10) > 0) + ((opc & 0x08) > 0);
            result.jump_address = get(DReg::hl);
            break;
        }
        case jump_stack::addr_modes::IMM ://JP u16
        {
            word address = fetched[1], hi = fetched[2];
            address += (hi << 8);
            byte opc = fetched[0];
            result.condition = 2 * ((opc & 0x10) > 0) + ((opc & 0x08) > 0);
            result.jump_address = address;
            break;
        }
        case jump_stack::addr_modes::REL ://JR e8
        {
            auto offset = static_cast<s_byte>(fetched.at(1));
            byte opc = fetched[0];

            result.condition = 2 * ((opc & 0x10) > 0) + ((opc & 0x08) > 0);
            result.jump_address = get(DReg::pc) + offset;

            if (opc == 0x30)
                result.condition = 2;
            break;
        }
        case jump_stack::addr_modes::RST ://RSTs
        {
            byte opc = fetched[0];
            result.jump_address = opc - 0xC7;
            break;
        }
        default:
            break;
    }
    return result;
}