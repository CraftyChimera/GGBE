//
// Created by drake on 25/9/22.
//

#include "Misc.hpp"
#include "Cpu.hpp"


const std::function<void(vector<Flag_Status> &, CPU *)> Misc::op_codes[9] = {Misc::CCF, Misc::CPL, Misc::DI, Misc::EI,
                                                                             Misc::NOP, Misc::SCF,
                                                                             Misc::HALT, Misc::STOP, Misc::DAA};

void Misc::dispatch(vector<Flag_Status> &flags, CPU *cpu, int op_id, [[maybe_unused]] vector<byte> &bytes_fetched,
                    [[maybe_unused]] int addr_mode) {
    Misc::op_codes[op_id](flags, cpu);
}

void Misc::CCF(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {
    bool flag_status = !flags.empty();
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, !flag_status));
}

void Misc::CPL(vector<Flag_Status> &flags, CPU *cpu) {
    byte A = cpu->get(Reg::a);
    A = ~A;
    flags.emplace_back(set(Flag::n, true));
    flags.emplace_back(set(Flag::h, true));
    cpu->set(Reg::a, A);
}

//TODO implement Interrupt instructions properly
void Misc::DI([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {}

void Misc::EI([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {}

void Misc::NOP([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {}

void Misc::SCF(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, true));
}

void Misc::STOP([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {
    //cpu->halt(true);
}

void Misc::HALT([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {}

void Misc::DAA([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu) {
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