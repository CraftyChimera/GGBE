//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CPU_HPP
#define DMGB_CPU_HPP

#include "../Base/Common.hpp"
#include "../Base/Utility.hpp"
#include "ALU.hpp"

class Console;

class CPU {
    word SP, PC;
    array<byte, 9> reg_mapper;
    array<Instructions, 2> Instruction_List;
    Console *game;
public:

    explicit CPU(Console *game);

    byte read(word address);

    void write(word address, byte value);

    byte get8(Reg reg_index);

    void set8(Reg reg_index, byte value);

    word get16(DReg reg_index);

    void set16(DReg reg_index_index, word value);

    byte getC();

    void setF(vector<Flag_Status> &flag_array);

    vector<byte> dispatch(int Type, int addr_mode, int bytes_to_fetch);

    void execute_op(vector<Flag_Status> &flags, int Type, int op_id, vector<byte> &args);

    //void ADC(vector<Flag_Status> &flags, byte value);

    //void ADD(vector<Flag_Status> &flags, byte value);

    // void ADD(vector<Flag_Status> &flags, word value);

    //void ADD(vector<Flag_Status> &flags, s_word value);
};


#endif //DMGB_CPU_HPP