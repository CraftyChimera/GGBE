//
// Created by drake on 4/9/22.
//

#ifndef DMGB_UNARY_HPP
#define DMGB_UNARY_HPP

#include "Utility.hpp"

class Cpu;

namespace Unary {

    struct op_args {
        std::variant<Reg, word> location;
        byte value;

        op_args();
    };

    Unary::op_args get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode);

    void dispatch(vector<Flag_Status> &, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addressing_mode);

    void Un_16(word &value, int op_id);

    byte INC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte DEC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte SWAP(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RL(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RLA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RLC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RLCA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RL_helper(vector<Flag_Status> &flags, Unary::op_args arg, byte lsb);

    byte RR(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RRA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RRC(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RRCA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte RR_helper(vector<Flag_Status> &flags, Unary::op_args arg, byte lsb);

    byte SLA(vector<Flag_Status> &flags, Unary::op_args arg);

    byte SRL(vector<Flag_Status> &flags, Unary::op_args arg);

    extern const std::function<byte(vector<Flag_Status> &, Unary::op_args)> op_codes[13] = {INC, DEC, SWAP, RL, RLA,
                                                                                            RLC, RLCA,
                                                                                            RR, RRA, RRC, RRCA, SLA,
                                                                                            SRL};
}
#endif //DMGB_UNARY_HPP
