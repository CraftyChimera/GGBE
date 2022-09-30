//
// Created by drake on 25/9/22.
//

#ifndef DMGB_INSTRUCTIONS_HPP
#define DMGB_INSTRUCTIONS_HPP

#include "../Base/Common.hpp"
#include "Utility.hpp"

struct Instructions {
    int Type;
    int op_id;
    int addr_mode;
    int bytes_to_fetch;
    int cycles;

    Instructions() noexcept;

    Instructions(int Type, int op_id, int addr_mode, int bytes_to_fetch, int cycles) noexcept;
};

extern const Instructions Instruction_List[256];
extern const Instructions Prefix_List[256];

#endif //DMGB_INSTRUCTIONS_HPP
