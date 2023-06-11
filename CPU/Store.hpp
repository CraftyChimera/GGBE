//
// Created by drake on 24/9/22.
//

#ifndef GGBE_STORE_HPP
#define GGBE_STORE_HPP

#include "Utility.hpp"

class CPU;
namespace Store {

    void dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, store::addr_modes addr_mode);

}

#endif //GGBE_STORE_HPP
