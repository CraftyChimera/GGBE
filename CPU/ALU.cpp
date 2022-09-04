//
// Created by drake on 2/9/22.
//

#include "ALU.hpp"
#include "Cpu.hpp"

vector<byte> ALU::dispatch(CPU *cpu, vector<byte> bytes_fetched, int addressing_mode) {
    switch (addressing_mode) {
        case 0: //ADC A,n8
            return vector<byte>{bytes_fetched[1]};
        case 1: //ADC A,r8
            return vector<byte>{cpu->get8(static_cast<Reg>(bytes_fetched[0] & 0xF))};
        case 2:// ADC A,[HL]
            return vector<byte>{cpu->read(cpu->get16(DReg::hl))};
        default:
            return vector<byte>{0xFF};
    }
}

void ALU::ADC(vector<Flag_Status> &flags, byte value, byte &A) {
    byte carry = flags.size();
    flags.clear();
    value += carry;

    if ((value == 0) && (carry != 0)) {
        flags.emplace_back(set(Flag::c));
        flags.emplace_back(set(Flag::h));
    }

    ALU::ADD(flags, value, A);
}


void ALU::ADD(vector<Flag_Status> &flags, byte value, byte &A) {
    word temp = A;

    if (temp + value > 0xFF)
        flags.emplace_back(set(Flag::c));

    if ((temp & 0xF) + (value & 0xF) > 0xF)
        flags.emplace_back(set(Flag::h));

    if ((temp & 0xFF) == 0)
        flags.emplace_back(set(Flag::z));

    flags.emplace_back(uset(Flag::n));
    A = static_cast<byte>((temp + value) & 0xFF);
}

void ALU::AND(vector<Flag_Status> &flags, byte value, byte &A) {
    A = A & value;
    if (A == 0)
        flags.emplace_back(set(Flag::z));
    flags.emplace_back(uset(Flag::n));
    flags.emplace_back(set(Flag::h));
    flags.emplace_back(uset(Flag::c));
}

void ALU::CP(vector<Flag_Status> &flags, byte value, byte &A) {
    if (value > A)
        flags.emplace_back(set(Flag::c));
    if ((value & 0xF) > (A & 0xF))
        flags.emplace_back(set(Flag::h));
    if (value == A)
        flags.emplace_back(set(Flag::z));
    flags.emplace_back(set(Flag::n));
}

void ALU::OR(vector<Flag_Status> &flags, byte value, byte &A) {
    A = A | value;
    if (A == 0)
        flags.emplace_back(set(Flag::z));
    flags.emplace_back(uset(Flag::n));
    flags.emplace_back(uset(Flag::h));
    flags.emplace_back(uset(Flag::c));
}

void ALU::SBC(vector<Flag_Status> &flags, byte value, byte &A) {
    byte carry = flags.size();
    flags.clear();
    value += carry;

    if ((value == 0) && (carry != 0)) {
        flags.emplace_back(set(Flag::c));
        flags.emplace_back(set(Flag::h));
    }

    ALU::SUB(flags, value, A);
}

void ALU::SUB(vector<Flag_Status> &flags, byte value, byte &A) {
    ALU::CP(flags, value, A);
    A -= value;
}

void ALU::XOR(vector<Flag_Status> &flags, byte value, byte &A) {
    A = A ^ value;
    if (A == 0)
        flags.emplace_back(set(Flag::z));
    flags.emplace_back(uset(Flag::n));
    flags.emplace_back(uset(Flag::h));
    flags.emplace_back(uset(Flag::c));
}