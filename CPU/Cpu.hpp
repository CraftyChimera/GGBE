//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CPU_HPP
#define DMGB_CPU_HPP

#endif //DMGB_CPU_HPP

#include "../Base/Common.hpp"

class CPU {
    byte A, B, C, D, E, H, L;
    half_word AF, BC, DE, HL, SP, PC;
public:
    CPU();
};