//
// Created by drake on 2/9/22.
//

#ifndef DMGB_ALU_HPP
#define DMGB_ALU_HPP

#include "../Base/Common.hpp"
#include "../Base/Utility.hpp"

class CPU;
namespace ALU {
    vector<byte> dispatch(CPU *cpu, vector<byte> bytes_fetched, int addressing_mode);

    void ADC(vector<Flag_Status> &, byte value, byte &);

    void ADD(vector<Flag_Status> &, byte value, byte &);

    void AND(vector<Flag_Status> &, byte value, byte &);

    void CP(vector<Flag_Status> &, byte value, byte &);

    void OR(vector<Flag_Status> &, byte value, byte &);

    void SBC(vector<Flag_Status> &, byte value, byte &);

    void SUB(vector<Flag_Status> &, byte value, byte &);

    void XOR(vector<Flag_Status> &, byte value, byte &);

    std::function<void(vector<Flag_Status> &, byte, byte &)> OPcodes[8] = {ADC, ADD, AND, CP, OR, SBC, SUB, XOR};
}
#endif //DMGB_ALU_HPP

