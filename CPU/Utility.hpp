//
// Created by drake on 14/9/22.
//

#ifndef DMGB_UTILITY_HPP
#define DMGB_UTILITY_HPP

#include "../Base/Common.hpp"

enum struct Reg {
    b = 0,
    c = 1,
    d = 2,
    e = 3,
    h = 4,
    l = 5,
    a = 7,
    f = 8
};

enum struct DReg {
    bc = 0,
    de = 2,
    hl = 4,
    af = 7,
    sp = 6,
    pc
};

enum Flag {
    z = 7,
    n = 6,
    h = 5,
    c = 4
};


struct Flag_Status {
    Flag bit;
    bool status;

    Flag_Status(Flag x, bool stat) : bit(x), status(stat) {};
};

Flag_Status set(Flag bit, bool status);

#endif //DMGB_UTILITY_HPP
