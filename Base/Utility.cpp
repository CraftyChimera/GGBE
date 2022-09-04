//
// Created by drake on 4/9/22.
//

#include "Utility.hpp"

Flag_Status set(Flag bit) {
    return {bit, true};
}

Flag_Status uset(Flag bit) {
    return {bit, false};
}

Instructions::Instructions() {
    Type = 0;
    op_id = 0;
    addr_mode = 0;
    bytes_to_fetch = 0;
    cycles = 0;
}

Instructions::Instructions(int Type, int op_id, int addr_mode, int bytes_to_fetch, int cycles) : Type(Type),
                                                                                                 op_id(op_id),
                                                                                                 addr_mode(addr_mode),
                                                                                                 bytes_to_fetch(
                                                                                                         bytes_to_fetch),
                                                                                                 cycles(cycles) {};

Instructions::Instructions(Instructions &src) {
    this->Type = src.Type;
    this->op_id = src.op_id;
    this->addr_mode = src.addr_mode;
    this->bytes_to_fetch = src.bytes_to_fetch;
    this->cycles = src.cycles;
}