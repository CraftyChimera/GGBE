//
// Created by drake on 27/8/22.
//

#include "Cpu.hpp"

CPU::CPU() {
    auto zero = static_cast<half_word>(0);
    reg_mapper.resize(8, static_cast<byte>(0));
    SP = zero;
    PC = zero;
}

byte CPU::get8(Reg reg) {
    auto reg_index = static_cast<size_t>(reg);
    return reg_mapper[reg_index];
}

void CPU::set8(Reg reg, byte value) {
    auto reg_index = static_cast<size_t>(reg);
    reg_mapper[reg_index] = value;
}

half_word CPU::get16(Reg reg) {
    auto reg_index = static_cast<size_t>(reg);
    return (reg_mapper[reg_index] << 8) + reg_mapper[reg_index + 1];
}

void CPU::set16(Reg reg, half_word value) {
    auto reg_index = static_cast<size_t>(reg);
    reg_mapper[reg_index + 1] = value & (0xFF);
    value = value >> 8;
    reg_mapper[reg_index] = value & (0xFF);
}

void CPU::setF(bool set, Flag bit) {
    auto bit_pos = static_cast<int>(bit);
    byte bitmask = 0xFF - (1 >> bit_pos);
    if (set)
        reg_mapper[1] |= (1 >> bit_pos);
    else
        reg_mapper[1] &= bitmask;
}