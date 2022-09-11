//
// Created by drake on 2/9/22.
//

#ifndef DMGB_ARITHMETIC_HPP
#define DMGB_ARITHMETIC_HPP

#include "../Base/Common.hpp"
#include "../Base/Utility.hpp"

class Cpu;
namespace Arithmetic {
    struct args {
        byte value;

        args();
    };

    args dispatch(Cpu *cpu, vector<byte> bytes_fetched, int addressing_mode);

    byte ADC(vector<Flag_Status> &, byte value, byte A);

    byte ADD(vector<Flag_Status> &, byte value, byte A);

    byte AND(vector<Flag_Status> &, byte value, byte A);

    byte CP(vector<Flag_Status> &, byte value, byte A);

    byte OR(vector<Flag_Status> &, byte value, byte A);

    byte SBC(vector<Flag_Status> &, byte value, byte A);

    byte SUB(vector<Flag_Status> &, byte value, byte A);

    byte XOR(vector<Flag_Status> &, byte value, byte A);

    const std::function<byte(vector<Flag_Status> &, byte, byte)> op_codes[8] = {ADC, ADD, AND, CP, OR, SBC, SUB, XOR};
}
#endif //DMGB_ARITHMETIC_HPP

