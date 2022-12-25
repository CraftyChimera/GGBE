//
// Created by drake on 25/9/22.
//

#include "Misc.hpp"
#include "Cpu.hpp"


/*const std::function<void(vector<Flag_Status> &, CPU *)> Misc::op_codes[9] = {Misc::CCF, Misc::CPL, Misc::DI, Misc::EI,
                                                                             Misc::NOP, Misc::SCF,
                                                                             Misc::HALT, Misc::STOP, Misc::DAA};
                                                                             */

void Misc::dispatch(vector<Flag_Status> &flags, CPU *cpu, int op_id) {
    switch (op_id) {
        case misc::op::CCF:
            Misc::CCF(flags);
            return;
        case misc::op::CPL:
            Misc::CPL(flags, cpu);
            return;
        case misc::op::DI:
            Misc::DI(cpu);
            return;
        case misc::op::EI:
            Misc::EI(cpu);
            return;
        case misc::op::NOP:
            Misc::NOP();
            return;
        case misc::op::SCF:
            Misc::SCF(flags);
            return;
        case misc::op::HALT:
            Misc::HALT(cpu);
            return;
        case misc::op::STOP:
            Misc::STOP(cpu);
            return;
        case misc::op::DAA:
            Misc::DAA(flags, cpu);
            return;
        default:
            return;
    }
}

void Misc::DAA(vector<Flag_Status> &flags, CPU *cpu) {
    byte flag = cpu->get(Reg::f);
    byte a_reg = cpu->get(Reg::a);

    byte n_flag = flag & (1 << Flag::n);
    byte c_flag = flag & (1 << Flag::c);
    byte h_flag = flag & (1 << Flag::h);

    if (!n_flag) {
        if (c_flag || a_reg > 0x99) {
            a_reg += 0x60;
            flags.emplace_back(set(Flag::c, true));
        }
        if (h_flag || (a_reg & 0x0f) > 0x09) {
            a_reg += 0x6;
        }
    } else {
        if (c_flag) { a_reg -= 0x60; }
        if (h_flag) { a_reg -= 0x6; }
    }
    a_reg &= 0xFF;
    flags.emplace_back(set(Flag::z, (a_reg == 0)));
    flags.emplace_back(set(Flag::h, false));
    cpu->set(Reg::a, a_reg);
}

void Misc::CPL(vector<Flag_Status> &flags, CPU *cpu) {
    byte A = cpu->get(Reg::a);

    bool n_bit = true;
    bool h_bit = true;

    flags = {
            set(Flag::n, n_bit),
            set(Flag::h, h_bit)
    };

    A = ~A;
    cpu->set(Reg::a, A);
}

void Misc::CCF(vector<Flag_Status> &flags) {
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = flags.empty();

    flags = {
            set(Flag::n, n_bit),
            set(Flag::h, h_bit),
            set(Flag::c, c_bit)
    };
}

void Misc::SCF(vector<Flag_Status> &flags) {

    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = true;

    flags = {
            set(Flag::n, n_bit),
            set(Flag::h, h_bit),
            set(Flag::c, c_bit)
    };
}

void Misc::HALT([[maybe_unused]] CPU *cpu) {}

void Misc::STOP([[maybe_unused]] CPU *cpu) {}

//TODO implement Interrupt instructions properly
void Misc::DI([[maybe_unused]] CPU *cpu) {}

void Misc::EI([[maybe_unused]] CPU *cpu) {}

void Misc::NOP() {}