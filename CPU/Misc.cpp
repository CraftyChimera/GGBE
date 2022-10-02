//
// Created by drake on 25/9/22.
//

#include "Misc.hpp"
#include "Cpu.hpp"


const std::function<void(vector<Flag_Status> &, Cpu *)> Misc::op_codes[9] = {Misc::CCF, Misc::CPL, Misc::DI, Misc::EI,
                                                                             Misc::NOP, Misc::SCF,
                                                                             Misc::HALT, Misc::STOP, Misc::DAA};

void Misc::dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, [[maybe_unused]] vector<byte> &bytes_fetched,
                    [[maybe_unused]] int addr_mode) {
    Misc::op_codes[op_id](flags, cpu);
}

void Misc::CCF(vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {
    bool flag_status = !flags.empty();
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, !flag_status));
}

void Misc::CPL(vector<Flag_Status> &flags, Cpu *cpu) {
    byte A = cpu->get(Reg::a);
    A = ~A;
    flags.emplace_back(set(Flag::n, true));
    flags.emplace_back(set(Flag::h, true));
    cpu->set(Reg::a, A);
}

//TODO implement Interrupt instructions properly
void Misc::DI([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {}

void Misc::EI([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {}

void Misc::NOP([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {}

void Misc::SCF(vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, true));
}

void Misc::STOP([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {
    //cpu->halt(true);
}

void Misc::HALT([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {}

void Misc::DAA([[maybe_unused]]vector<Flag_Status> &flags, [[maybe_unused]] Cpu *cpu) {}