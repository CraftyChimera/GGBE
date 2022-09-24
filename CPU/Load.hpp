//
// Created by drake on 17/9/22.
//

#ifndef DMGB_LOAD_HPP
#define DMGB_LOAD_HPP

#include "Utility.hpp"

class Cpu;
namespace Load {

    struct op_args {
        byte src_value;
        Reg destination;

        op_args();
    };

    void dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode);

    Load::op_args get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode);
}

#endif //DMGB_LOAD_HPP
