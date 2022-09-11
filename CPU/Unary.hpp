//
// Created by drake on 4/9/22.
//

#ifndef DMGB_UNARY_HPP
#define DMGB_UNARY_HPP

#include "../Base/Common.hpp"
#include "../Base/Utility.hpp"

class Cpu;

namespace Unary {

    struct args {
        std::variant<Reg, word> source;
        byte value;

        args();
    };

    args dispatch(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode);

    byte INC(vector<Flag_Status> &flags, byte value);

    byte DEC(vector<Flag_Status> &flags, byte value);

    byte SWAP(vector<Flag_Status> &flags, byte value);

    byte RL(vector<Flag_Status> &flags, byte value);

    byte RLA(vector<Flag_Status> &flags, byte value);

    byte RLC(vector<Flag_Status> &flags, byte value);

    byte RLCA(vector<Flag_Status> &flags, byte value);

    byte RL_helper(vector<Flag_Status> &flags, byte value, byte lsb);

    byte RR(vector<Flag_Status> &flags, byte value);

    byte RRA(vector<Flag_Status> &flags, byte value);

    byte RRC(vector<Flag_Status> &flags, byte value);

    byte RRCA(vector<Flag_Status> &flags, byte value);

    byte RR_helper(vector<Flag_Status> &flags, byte value, byte lsb);

    byte SLA(vector<Flag_Status> &flags, byte value);

    byte SRL(vector<Flag_Status> &flags, byte value);

    const std::function<byte(vector<Flag_Status> &, byte)> op_codes[13] = {INC, DEC, SWAP, RL, RLA, RLC, RLCA,
                                                                           RR, RRA, RRC, RRCA, SLA, SRL};
}
#endif //DMGB_UNARY_HPP
