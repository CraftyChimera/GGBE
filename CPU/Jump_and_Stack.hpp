//
// Created by drake on 24/9/22.
//

#ifndef DMGB_JUMP_AND_STACK_HPP
#define DMGB_JUMP_AND_STACK_HPP

#include "Utility.hpp"

class CPU;

namespace Jump {
    struct op_args {
        word jump_address;
        int condition;

        op_args();
    };

    Jump::op_args get_args(CPU *cpu, vector<byte> &bytes_fetched, jump_stack::addr_modes addressing_mode);

    void dispatch(CPU *cpu, int op_id, vector<byte> &bytes_fetched, jump_stack::addr_modes addr_mode);

    void JP(CPU *cpu, Jump::op_args &args);

    void JPC(CPU *cpu, Jump::op_args &args);

    void CALL(CPU *cpu, Jump::op_args &args);

    void CALLC(CPU *cpu, Jump::op_args &args);

    void RET(CPU *cpu, Jump::op_args &args);

    void RETC(CPU *cpu, Jump::op_args &args);

    void RETI(CPU *cpu, Jump::op_args &args);

    void POP(CPU *cpu, Jump::op_args &args);

    void PUSH(CPU *cpu, Jump::op_args &args);

    extern const std::function<void(CPU *, Jump::op_args &)> op_codes[9];

}
#endif //DMGB_JUMP_AND_STACK_HPP
