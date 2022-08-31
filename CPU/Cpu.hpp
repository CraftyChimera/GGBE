//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CPU_HPP
#define DMGB_CPU_HPP

#include "../Base/Common.hpp"

class CPU {
    vector<byte> reg_mapper;
    half_word SP, PC;
public:
    CPU();

    byte get8(Reg reg);

    void set8(Reg reg, byte value);

    half_word get16(Reg reg);

    void set16(Reg reg, half_word value);

    void setF(bool set, Flag bit);

};

#endif //DMGB_CPU_HPP