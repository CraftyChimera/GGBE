//
// Created by drake on 24/9/22.
//

#ifndef DMGB_JUMP_AND_STACK_HPP
#define DMGB_JUMP_AND_STACK_HPP

#include "Utility.hpp"

class Cpu;

namespace Jump {
    struct op_args {
        word jump_address;
        int condition;

        op_args();
    };

    Jump::op_args get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode);

    void dispatch(vector<Flag_Status> &, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode);

    void JP(Cpu *cpu, Jump::op_args &args);

    void JPC(Cpu *cpu, Jump::op_args &args);

    void CALL(Cpu *cpu, Jump::op_args &args);

    void CALLC(Cpu *cpu, Jump::op_args &args);

    void RET(Cpu *cpu, Jump::op_args &args);

    void RETC(Cpu *cpu, Jump::op_args &args);

    void RETI(Cpu *cpu, Jump::op_args &args);

    void POP(Cpu *cpu, Jump::op_args &args);

    void PUSH(Cpu *cpu, Jump::op_args &args);

    extern const std::function<void(Cpu *, Jump::op_args &)> op_codes[9];

}
#endif //DMGB_JUMP_AND_STACK_HPP
