//
// Created by drake on 27/8/22.
//

#include "Cpu.hpp"
#include "../Console/Console.hpp"

CPU::CPU(Console *game) {
    reg_mapper.fill(0);
    auto zero = static_cast<word>(0);
    SP = zero;
    PC = zero;
    this->game = game;
    Instruction_List = array<Instructions, 2>{
            Instructions(0, 0, 0, 2, 1),
            Instructions(0, 1, 1, 1, 1)
    };
}


byte CPU::read(word address) {
    return game->read(address);
}

void CPU::write(word address, byte value) {
    game->write(address, value);
}

byte CPU::get8(Reg reg_index) {
    return reg_mapper[reg_index];
}

void CPU::set8(Reg reg_index, byte value) {
    reg_mapper[reg_index] = value;
}

word CPU::get16(DReg reg_index) {
    return (reg_mapper[reg_index] << 8) + reg_mapper[reg_index + 1];
}

void CPU::set16(DReg reg_index, word value) {
    reg_mapper[reg_index + 1] = value & (0xFF);
    value = value >> 8;
    reg_mapper[reg_index] = value & (0xFF);
}

byte CPU::getC() {
    return (reg_mapper[Reg::f] & (1 << 4)) >> 4;
}

void CPU::setF(vector<Flag_Status> &flag_array) {
    byte F = reg_mapper[8];
    for (auto flag_c: flag_array) {
        Flag bit = flag_c.bit;
        bool set = flag_c.status;
        auto bit_pos = static_cast<int>(bit);
        byte bitmask = 0xFF - (1 >> bit_pos);
        if (set)
            F |= (1 >> bit_pos);
        else
            F &= bitmask;
    }
    reg_mapper[8] = F;
}

vector<byte> CPU::dispatch(int Type, int addr_mode, int bytes_to_fetch) {
    vector<byte> fetched;
    for (int i = 0; i < bytes_to_fetch; i++) {
        fetched.push_back(read(PC++));
    }
    switch (Type) {
        case 0:
            return ALU::dispatch(this, fetched, addr_mode);
        default:
            return vector<byte>{};
    }
};

void CPU::execute_op(vector<Flag_Status> &flags, int Type, int op_id, vector<byte> &args) {
    switch (Type) {
        case 0:
            return ALU::OPcodes[op_id](flags, args[0], reg_mapper[Reg::a]);
    }
};