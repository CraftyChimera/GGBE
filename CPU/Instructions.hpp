//
// Created by drake on 25/9/22.
//

#ifndef DMGB_INSTRUCTIONS_HPP
#define DMGB_INSTRUCTIONS_HPP

#include "Utility.hpp"

struct Instructions {
    Type instr_type;
    int op_id;
    int addr_mode;
    int bytes_to_fetch;
    int cycles;

    Instructions(Type type, int op_id, int addr_mode, int bytes_to_fetch, int cycles) noexcept;
};

extern const Instructions Instruction_List[256];
extern const Instructions Prefix_List[256];

#endif //DMGB_INSTRUCTIONS_HPP
