//
// Created by drake on 2/9/22.
//

#ifndef DMGB_ARITHMETIC_HPP
#define DMGB_ARITHMETIC_HPP

#include "Utility.hpp"

class Cpu;
namespace Arithmetic {
    struct op_args {
        byte value;
        byte src_value;

        op_args();
    };

    op_args get_args(Cpu *cpu, vector<byte> bytes_fetched, int addressing_mode);

    void dispatch(vector<Flag_Status> &, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addressing_mode);

    byte ADD(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte ADC(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte AND(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte CP(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte OR(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte SBC(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte SUB(vector<Flag_Status> &, Arithmetic::op_args arg);

    byte XOR(vector<Flag_Status> &, Arithmetic::op_args arg);

    extern const std::function<byte(vector<Flag_Status> &, Arithmetic::op_args)> op_codes[8];
}
#endif //DMGB_ARITHMETIC_HPP

