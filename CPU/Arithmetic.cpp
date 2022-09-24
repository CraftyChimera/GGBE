//
// Created by drake on 2/9/22.
//

#include "Arithmetic.hpp"
#include "Cpu.hpp"

Arithmetic::op_args::op_args() {
    value = 0;
    src_value = 0;
}

word Arithmetic::ADD_16(vector<Flag_Status> &flags, word src, word addend) {
    d_word temp = src + addend;

    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, ((src & 0xFFF) + (addend & 0xFFF) > 0xFFF)));
    flags.emplace_back(set(Flag::c, (temp > 0xFFFF)));

    return static_cast<word>(temp & 0xFFFF);
}

void Arithmetic::dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {

    if (addr_mode == -1) {

        word src = cpu->get(DReg::hl), addend, result;

        int bitmask = (bytes_fetched[0] >> 4);
        DReg reg_index = static_cast<DReg>(2 * bitmask);

        addend = cpu->get(reg_index);
        result = ADD_16(flags, src, addend);
        cpu->set(DReg::hl, result);
        return;
    }

    auto args = Arithmetic::get_args(cpu, bytes_fetched, addr_mode);
    byte result = Arithmetic::op_codes[op_id](flags, args);
    cpu->set(Reg::a, result);
}

Arithmetic::op_args Arithmetic::get_args(Cpu *cpu, vector<byte> bytes_fetched, int addressing_mode) {
    Arithmetic::op_args result;
    result.src_value = cpu->get(Reg::a);
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

byte Arithmetic::ADD(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte value = arg.value, src = arg.src_value;
    word temp = arg.src_value + arg.value;

    flags.emplace_back(set(Flag::z, ((temp & 0xFF) == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, ((src & 0xF) + (value & 0xF) > 0xF)));
    flags.emplace_back(set(Flag::c, (temp > 0xFF)));

    return static_cast<byte>(temp & 0xFF);
}

byte Arithmetic::ADC(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A, carry = flags.size();

    arg.value += carry;

    A = Arithmetic::ADD(flags, arg);

    if ((arg.value == 0) && (carry != 0)) {
        flags.emplace_back(set(Flag::h, true));
        flags.emplace_back(set(Flag::c, true));
    }
    return A;
}

byte Arithmetic::AND(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A = arg.src_value, value = arg.value;
    A = A & value;
    flags.emplace_back(set(Flag::z, (A == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, true));
    flags.emplace_back(set(Flag::c, false));
    return A;
}

byte Arithmetic::CP(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A = arg.src_value, value = arg.value;
    flags.emplace_back(set(Flag::z, (value == A)));
    flags.emplace_back(set(Flag::n, true));
    flags.emplace_back(set(Flag::h, ((value & 0xF) > (A & 0xF))));
    flags.emplace_back(set(Flag::c, (value > A)));
    return A;
}

byte Arithmetic::OR(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A = arg.src_value, value = arg.value;
    A = A | value;
    flags.emplace_back(set(Flag::z, (A == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, false));
    return A;
}

byte Arithmetic::SUB(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A, value = arg.value;
    A = Arithmetic::CP(flags, arg);
    A -= value;
    return A;
}

byte Arithmetic::SBC(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A, carry = flags.size();

    arg.value += carry;

    A = Arithmetic::SUB(flags, arg);

    if ((arg.value == 0) && (carry != 0)) {
        flags.emplace_back(set(Flag::c, true));
        flags.emplace_back(set(Flag::h, true));
    }

    return A;
}


byte Arithmetic::XOR(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte A = arg.src_value, value = arg.value;
    A = A ^ value;
    flags.emplace_back(set(Flag::z, (A == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, false));
    return A;
}