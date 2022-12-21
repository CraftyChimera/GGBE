//
// Created by drake on 24/9/22.
//

#ifndef DMGB_STORE_HPP
#define DMGB_STORE_HPP

#include "Utility.hpp"

class CPU;
namespace Store {

    void dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, store::addr_modes addr_mode);

}

#endif //DMGB_STORE_HPP
