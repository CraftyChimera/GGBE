//
// Created by drake on 25/9/22.
//

#ifndef DMGB_MISC_HPP
#define DMGB_MISC_HPP

#include "Utility.hpp"

class CPU;

namespace Misc {

    void dispatch(vector<Flag_Status> &, CPU *cpu, int op_id);

    void DAA(vector<Flag_Status> &flags, CPU *cpu);

    void CPL(vector<Flag_Status> &flags, CPU *cpu);

    void CCF(vector<Flag_Status> &flags);

    void SCF(vector<Flag_Status> &flags);

    void HALT(CPU *cpu);

    void STOP(CPU *cpu);

    void DI(CPU *cpu);

    void EI(CPU *cpu);

    void NOP();
}

#endif //DMGB_MISC_HPP
