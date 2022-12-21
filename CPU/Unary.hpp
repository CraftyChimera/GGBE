//
// Created by drake on 4/9/22.
//

#ifndef DMGB_UNARY_HPP
#define DMGB_UNARY_HPP

#include "Utility.hpp"

class CPU;

namespace Unary {

    struct op_args {
        std::variant<Reg, word> location;
        byte value;

        op_args();

        op_args(Reg reg, byte value);

        op_args(word address, byte value);

    };

    Unary::op_args get_args_cb(CPU *cpu, vector<byte> &bytes_fetched, unary::addr_modes addressing_mode);

    Unary::op_args get_args_non_cb(CPU *cpu, vector<byte> &bytes_fetched);

    Unary::op_args get_args(CPU *cpu, int op_id, vector<byte> &bytes_fetched, unary::addr_modes addr_mode);

    void dispatch(vector<Flag_Status> &, CPU *cpu, int op_id, vector<byte> &bytes_fetched,
                  unary::addr_modes addressing_mode);

    void DI_r16(CPU *cpu, vector<byte> &bytes_fetched, int op_id);

    byte INC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte DEC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte SWAP(vector<Flag_Status> &flags, Unary::op_args arg);

    // msb/lsb is 0
    byte SLA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte SRL(vector<Flag_Status> &flags, Unary::op_args arg);

    // msb/lsb is carry bit
    byte RL(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RR(vector<Flag_Status> &flags, Unary::op_args arg);

    // lsb/msb is bit-7/bit-0 - Rotate bits
    byte RLC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RRC(vector<Flag_Status> &flags, Unary::op_args arg);

    // duplicate msb/lsb
    byte SRA(vector<Flag_Status> &flags, Unary::op_args arg);

    //Special RL,RR,RLC,RRC variants for A
    byte RLA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RRA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RLCA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RRCA(vector<Flag_Status> &flags, Unary::op_args arg);


    extern const std::function<byte(vector<Flag_Status> &, Unary::op_args)> op_codes[14];
}
#endif //DMGB_UNARY_HPP
