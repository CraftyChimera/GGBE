//
// Created by drake on 24/9/22.
//

#ifndef DMGB_STORE_HPP
#define DMGB_STORE_HPP

#include "Utility.hpp"

class Cpu;
namespace Store {

    struct op_args {
        byte src_value;
        word destination;

        op_args();
    };

    void dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode);

    Store::op_args get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode);
}

#endif //DMGB_STORE_HPP
