//
// Created by drake on 11/9/22.
//

#ifndef DMGB_BIT_OPERATIONS_HPP
#define DMGB_BIT_OPERATIONS_HPP

#include "Utility.hpp"

class CPU;
namespace Bit_Operations {

    struct op_args {
        byte test_bit, value;
        std::variant<Reg, word> location;

        op_args();
    };

    Bit_Operations::op_args get_args(CPU *cpu, vector<byte> &bytes_fetched, int addressing_mode);

    void dispatch(vector<Flag_Status> &, CPU *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode);

    word BIT(vector<Flag_Status> &, Bit_Operations::op_args);

    word RES(vector<Flag_Status> &, Bit_Operations::op_args);

    word SET(vector<Flag_Status> &, Bit_Operations::op_args);

    extern const std::function<byte(vector<Flag_Status> &, Bit_Operations::op_args)> op_codes[3];
}

#endif //DMGB_BIT_OPERATIONS_HPP
