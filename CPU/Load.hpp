//
// Created by drake on 17/9/22.
//

#ifndef DMGB_LOAD_HPP
#define DMGB_LOAD_HPP

#include "Utility.hpp"

class CPU;
namespace Load {
    void dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode);
}

#endif //DMGB_LOAD_HPP
