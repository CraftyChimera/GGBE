//
// Created by drake on 2/9/22.
//

#include "Arithmetic.hpp"
#include "Cpu.hpp"

Arithmetic::args::args() {
    value = 0;
}

Arithmetic::args Arithmetic::dispatch(Cpu *cpu, vector<byte> bytes_fetched, int addressing_mode) {
    Arithmetic::args result;
    switch (addressing_mode) {
        case 0: //ADC A,n8
            result.value = bytes_fetched[1];
            break;
        case 1: //ADC A,r8
            result.value = cpu->get(static_cast<Reg>(bytes_fetched[0] & 0x7));
            break;
        case 2:// ADC A,[HL]
            result.value = cpu->read(cpu->get(DReg::hl));
            break;
        default:
            break;
    }
    return result;
}

byte Arithmetic::ADD(vector<Flag_Status> &flags, byte value, byte A) {
    word temp = A;

    flags.emplace_back(set(Flag::z, ((temp & 0xFF) == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, ((temp & 0xF) + (value & 0xF) > 0xF)));
    flags.emplace_back(set(Flag::c, (temp + value > 0xFF)));

    temp += value;
    A = static_cast<byte>(temp & 0xFF);
    return A;
}

byte Arithmetic::ADC(vector<Flag_Status> &flags, byte value, byte A) {
    byte carry = flags.size();
    value += carry;

    A = Arithmetic::ADD(flags, value, A);

    if ((value == 0) && (carry != 0)) {
        flags.emplace_back(set(Flag::h, true));
        flags.emplace_back(set(Flag::c, true));
    }
    return A;
}

byte Arithmetic::AND(vector<Flag_Status> &flags, byte value, byte A) {
    A = A & value;
    flags.emplace_back(set(Flag::z, (A == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, true));
    flags.emplace_back(set(Flag::c, false));
    return A;
}

byte Arithmetic::CP(vector<Flag_Status> &flags, byte value, byte A) {
    flags.emplace_back(set(Flag::z, (value == A)));
    flags.emplace_back(set(Flag::n, true));
    flags.emplace_back(set(Flag::h, ((value & 0xF) > (A & 0xF))));
    flags.emplace_back(set(Flag::c, (value > A)));
    return A;
}

byte Arithmetic::OR(vector<Flag_Status> &flags, byte value, byte A) {
    A = A | value;
    flags.emplace_back(set(Flag::z, (A == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, false));
    return A;
}

byte Arithmetic::SUB(vector<Flag_Status> &flags, byte value, byte A) {
    A = Arithmetic::CP(flags, value, A);
    A -= value;
    return A;
}

byte Arithmetic::SBC(vector<Flag_Status> &flags, byte value, byte A) {
    byte carry = flags.size();
    value += carry;

    A = Arithmetic::SUB(flags, value, A);

    if ((value == 0) && (carry != 0)) {
        flags.emplace_back(set(Flag::c, true));
        flags.emplace_back(set(Flag::h, true));
    }

    return A;
}


byte Arithmetic::XOR(vector<Flag_Status> &flags, byte value, byte A) {
    A = A ^ value;
    flags.emplace_back(set(Flag::z, (A == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, false));
    return A;
}