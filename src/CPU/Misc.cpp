//
// Created by drake on 25/9/22.
//

#include "Cpu.hpp"

void CPU::misc_dispatch(int op_id, [[maybe_unused]] misc::addr_modes &addr_mode) {
    switch (op_id) {
        case misc::op::CCF:
            CCF();
            return;
        case misc::op::CPL:
            CPL();
            return;
        case misc::op::DI:
            DI();
            return;
        case misc::op::EI:
            EI();
            return;
        case misc::op::NOP:
            NOP();
            return;
        case misc::op::SCF:
            SCF();
            return;
        case misc::op::HALT:
            HALT();
            return;
        case misc::op::STOP:
            STOP();
            return;
        case misc::op::DAA:
            DAA();
            return;
        default:
            return;
    }
}

void CPU::DAA() {
    byte flag = get(Reg::f);
    byte a_reg = get(Reg::a);

    byte n_flag = flag & (1 << Flag::n);
    byte c_flag = flag & (1 << Flag::c);
    byte h_flag = flag & (1 << Flag::h);
    vector<FlagStatus> new_flags;
    if (!n_flag) {
        if (c_flag || a_reg > 0x99) {
            a_reg += 0x60;
            new_flags.emplace_back(add_flag_status_change(Flag::c, true));
        }
        if (h_flag || (a_reg & 0x0f) > 0x09) {
            a_reg += 0x6;
        }
    } else {
        if (c_flag) { a_reg -= 0x60; }
        if (h_flag) { a_reg -= 0x6; }
    }
    a_reg &= 0xFF;
    new_flags.emplace_back(add_flag_status_change(Flag::z, (a_reg == 0)));
    new_flags.emplace_back(add_flag_status_change(Flag::h, false));

    set_flags(new_flags);
    set(Reg::a, a_reg);
}

void CPU::CPL() {
    byte A = get(Reg::a);

    bool n_bit = true;
    bool h_bit = true;

    auto new_flags = vector<FlagStatus>(
            {
                    add_flag_status_change(Flag::n, n_bit),
                    add_flag_status_change(Flag::h, h_bit)
            });
    set_flags(new_flags);

    A = ~A;
    set(Reg::a, A);
}

void CPU::CCF() {
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = !get_carry();

    auto new_flags = vector<FlagStatus>(
            {
                    add_flag_status_change(Flag::n, n_bit),
                    add_flag_status_change(Flag::h, h_bit),
                    add_flag_status_change(Flag::c, c_bit)
            });
    set_flags(new_flags);
}

void CPU::SCF() {

    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = true;

    auto new_flags = vector<FlagStatus>(
            {
                    add_flag_status_change(Flag::n, n_bit),
                    add_flag_status_change(Flag::h, h_bit),
                    add_flag_status_change(Flag::c, c_bit)
            });
    set_flags(new_flags);
}

void CPU::HALT() {
    halt_mode = true;
    if (is_interrupt_pending())
        halt_bug = true;
}

void CPU::STOP() {}

void CPU::DI() {
    interrupt_buffer = -1;
}

void CPU::EI() {
    interrupt_buffer = 2;
}

void CPU::NOP() {}