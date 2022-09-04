//
// Created by drake on 4/9/22.
//

#ifndef DMGB_UTILITY_HPP
#define DMGB_UTILITY_HPP

enum Reg {
    b = 0,
    c = 1,
    d = 2,
    e = 3,
    h = 4,
    l = 5,
    a = 7,
    f = 8
};

enum DReg {
    bc = 0,
    de = 2,
    hl = 4,
    af = 7
};

enum struct Flag {
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

Flag_Status set(Flag);

Flag_Status uset(Flag bit);

class Instructions {
    int Type;
    int op_id;
    int addr_mode;
    int bytes_to_fetch;
    int cycles;

public:
    Instructions();

    Instructions(int Type, int op_id, int addr_mode, int bytes_to_fetch, int cycles);

    Instructions(Instructions &src);
};


#endif //DMGB_UTILITY_HPP
