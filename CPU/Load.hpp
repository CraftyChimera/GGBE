//
// Created by drake on 17/9/22.
//

#ifndef GGBE_LOAD_HPP
#define GGBE_LOAD_HPP

#include "Utility.hpp"

class CPU;
namespace Load {
    void dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, load::addr_modes addr_mode);
}

#endif //GGBE_LOAD_HPP
