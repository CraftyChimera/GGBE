//
// Created by drake on 4/9/22.
//

#include "Utility.hpp"

Flag_Status set(Flag bit, bool status) {
    return {bit, status};
}

Instructions::Instructions() {
    Type = 0;
    op_id = 0;
    addr_mode = 0;
    bytes_to_fetch = 0;
    cycles = 0;
}

Instructions::Instructions(int Type, int op_id, int addr_mode, int bytes_to_fetch, int cycles) {
    this->Type = Type;
    this->op_id = op_id;
    this->addr_mode = addr_mode;
    this->bytes_to_fetch = bytes_to_fetch;
    this->cycles = cycles;
};