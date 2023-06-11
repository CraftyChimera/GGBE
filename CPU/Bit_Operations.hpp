//
// Created by drake on 11/9/22.
//

#ifndef GGBE_BIT_OPERATIONS_HPP
#define GGBE_BIT_OPERATIONS_HPP

#include "Utility.hpp"

class CPU;
namespace Bit_Operations {

    struct op_args {
        byte test_bit;
        byte value;
        std::variant<Reg, word> location;

        op_args();

        op_args(byte test_bit, byte value, Reg reg);

        op_args(byte test_bit, byte value, word address);
    };

    Bit_Operations::op_args get_args(CPU *cpu, vector<byte> &bytes_fetched, bit_op::addr_modes addressing_mode);

    void
    dispatch(vector<Flag_Status> &, CPU *cpu, int op_id, vector<byte> &bytes_fetched, bit_op::addr_modes addr_mode);

    byte BIT(vector<Flag_Status> &, Bit_Operations::op_args);

    byte RES(vector<Flag_Status> &, Bit_Operations::op_args);

    byte SET(vector<Flag_Status> &, Bit_Operations::op_args);

    extern const std::function<byte(vector<Flag_Status> &, Bit_Operations::op_args)> op_codes[3];
}

#endif //GGBE_BIT_OPERATIONS_HPP
