//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CPU_HPP
#define DMGB_CPU_HPP

#include "../Base/Common.hpp"
#include "../Base/Utility.hpp"
#include "Arithmetic.hpp"
#include "Unary.hpp"
#include "Bit_Test.hpp"

class Console;

class Cpu {
    word SP, PC;
    array<byte, 9> reg_mapper;
    array<Instructions, 2> Instruction_List;
    Console *game;
public:

    explicit Cpu(Console *game);

    byte read(word address);

    void write(word address, byte value);

    byte get(Reg reg_index);

    word get(DReg reg_index);

    void set(Reg reg_index, byte value);

    void set(DReg reg_index, word value);

    void set_flags(vector<Flag_Status> &flag_array);

    vector<byte> fetch(vector<Flag_Status> &flags, Instructions &instruction_data);

    void decode_and_execute(vector<Flag_Status> &flags, vector<byte> fetched, Instructions &instruction_data);

    void loop();
};

#endif //DMGB_CPU_HPP