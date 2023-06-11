//
// Created by drake on 25/9/22.
//

#ifndef DMGB_INSTRUCTIONS_HPP
#define DMGB_INSTRUCTIONS_HPP

#include "Utility.hpp"

using addr_type = std::variant<
        arithmetic::addr_modes,
        unary::addr_modes,
        bit_op::addr_modes,
        load::addr_modes,
        store::addr_modes,
        jump_stack::addr_modes,
        misc::addr_modes>;

struct Instructions {
    Type instr_type;
    int op_id{};
    addr_type addr_mode;
    int bytes_to_fetch{};

    Instructions() noexcept;

    Instructions(Type type, int op_id, addr_type addr_mode, int bytes_to_fetch) noexcept;
};

extern const Instructions Instruction_List[256];
extern const Instructions Prefix_List[256];

#endif //DMGB_INSTRUCTIONS_HPP
