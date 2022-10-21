//
// Created by drake on 25/9/22.
//

#ifndef DMGB_MISC_HPP
#define DMGB_MISC_HPP

#include "Utility.hpp"

class CPU;

namespace Misc {

    void
    dispatch(vector<Flag_Status> &, CPU *cpu, int op_id, vector<byte> &bytes_fetched, [[maybe_unused]] int addr_mode);

    void CCF(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu);

    void CPL(vector<Flag_Status> &flags, CPU *cpu);

    void DI(vector<Flag_Status> &flags, CPU *cpu);

    void EI(vector<Flag_Status> &flags, CPU *cpu);

    void NOP(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu);

    void SCF(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu);

    void HALT(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu);

    void STOP(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu);

    void DAA(vector<Flag_Status> &flags, [[maybe_unused]] CPU *cpu);

    extern const std::function<void(vector<Flag_Status> &, CPU *)> op_codes[9];
}

#endif //DMGB_MISC_HPP
