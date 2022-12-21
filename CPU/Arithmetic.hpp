//
// Created by drake on 2/9/22.
//

#ifndef DMGB_ARITHMETIC_HPP
#define DMGB_ARITHMETIC_HPP

#include "Utility.hpp"

class CPU;
namespace Arithmetic {
    struct op_args {
        byte src_value;
        byte value;

        op_args(byte src_value, byte value);
    };

    op_args get_args(CPU *cpu, vector<byte> bytes_fetched, arithmetic::addr_modes addressing_mode);

    void dispatch(vector<Flag_Status> &, CPU *cpu, int op_id, vector<byte> &bytes_fetched,
                  arithmetic::addr_modes addressing_mode);

    word ADD_16(vector<Flag_Status> &flags, word src, word addend);

    word ADD_TO_SP(vector<Flag_Status> &flags, word src, s_byte signed_offset);

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

