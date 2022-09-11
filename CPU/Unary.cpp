//
// Created by drake on 4/9/22.
//

#include "Unary.hpp"
#include "Cpu.hpp"

Unary::args::args() {
    source = static_cast<word>(0);
    value = 0;
}

Unary::args Unary::dispatch(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Unary::args result;
    switch (addressing_mode) {
        case 0: // INC r8
        {
            Reg reg_index = static_cast<Reg>(bytes_fetched[0] & 0x7);
            result.source = reg_index;
            result.value = cpu->get(reg_index);
            break;
        }
        case 1: //INC [HL]
        {
            word lo = bytes_fetched[0], hi = bytes_fetched[1], address;
            address = lo + (hi << 8);
            result.source = address;
            result.value = cpu->read(address);
            break;
        }
        default:
            break;
    }
    return result;
}

byte Unary::INC(vector<Flag_Status> &flags, byte value) {
    word temp = value + 1;

    flags.emplace_back(set(Flag::z, ((temp & 0xFF) == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, ((value & 0xF) == 0xF)));

    value = static_cast<byte>(temp & 0xFF);
    return value;
}

byte Unary::DEC(vector<Flag_Status> &flags, byte value) {
    word temp = value - 1;

    flags.emplace_back(set(Flag::z, ((temp & 0xFF) == 0)));
    flags.emplace_back(set(Flag::n, true));
    flags.emplace_back(set(Flag::h, ((value & 0xF) == 0)));

    value = static_cast<byte>(temp & 0xFF);
    return value;
}

byte Unary::SWAP(vector<Flag_Status> &flags, byte value) {
    word lo = value & 0xFF;
    value = value >> 8;
    word hi = value & 0xFF;
    value = hi + (lo << 8);

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, false));

    return value;
}

byte Unary::RL_helper(vector<Flag_Status> &flags, byte value, byte lsb) {
    word temp = value;
    (temp *= 2) += lsb;
    value = temp & 0xFF;

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, ((temp >> 8) > 0)));

    return value;
}

byte Unary::SLA(vector<Flag_Status> &flags, byte value) {
    return RL_helper(flags, value, 0);
}

byte Unary::RL(vector<Flag_Status> &flags, byte value) {
    byte carry = flags.size();
    return RL_helper(flags, value, carry);
}

byte Unary::RLA(vector<Flag_Status> &flags, byte value) {
    value = RL(flags, value);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RLC(vector<Flag_Status> &flags, byte value) {
    byte msb = value >> 7;
    return RL_helper(flags, value, msb);
}

byte Unary::RLCA(vector<Flag_Status> &flags, byte value) {
    value = RLC(flags, value);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RR_helper(vector<Flag_Status> &flags, byte value, byte lsb) {
    word temp = value;
    byte carry = temp & 0x01;
    (temp /= 2) += (lsb << 7);
    value = temp;

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, (carry == 1)));

    return value;
}

byte Unary::SRL(vector<Flag_Status> &flags, byte value) {
    return RR_helper(flags, value, 0);
}

byte Unary::RR(vector<Flag_Status> &flags, byte value) {
    byte carry = flags.size();
    return RR_helper(flags, value, carry);
}

byte Unary::RRA(vector<Flag_Status> &flags, byte value) {
    value = RR(flags, value);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RRC(vector<Flag_Status> &flags, byte value) {
    byte lsb = value & 0x1;
    return RR_helper(flags, value, lsb);
}

byte Unary::RRCA(vector<Flag_Status> &flags, byte value) {
    value = RRC(flags, value);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

