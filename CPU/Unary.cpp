//
// Created by drake on 4/9/22.
//

#include "Cpu.hpp"

CPU::unary_op_args::unary_op_args(Reg reg, byte value, bool carry) : location(reg), value(value), carry(carry) {}

CPU::unary_op_args::unary_op_args(word address, byte value, bool carry) : location(address), value(value),
                                                                          carry(carry) {}

void CPU::unary_dispatch(int op_id, unary::addr_modes addr_mode) {

    if (addr_mode == unary::addr_modes::REG_16) {
        DI_r16(op_id);
        tick_components();
        return;
    }

    auto args = get_unary_args(op_id, addr_mode);
    auto result = unary_op_codes[op_id](args);
    auto value = result.result_byte;
    auto new_flags = result.new_flag_status;
    set_flags(new_flags);


    switch (args.location.index()) {
        case 0: {
            Reg reg_index = std::get<0>(args.location);
            set(reg_index, value);
            break;
        }
        case 1: {
            word address = std::get<1>(args.location);
            write(address, value);
            break;
        }
    }
}

CPU::unary_op_args CPU::get_unary_args(int op_id, unary::addr_modes addr_mode) {
    bool carry = get_carry();
    switch (op_id) {
        case unary::op::INC:
        case unary::op::DEC:
            return CPU::get_unary_args_non_cb();

        case unary::op::RRA:
        case unary::op::RRCA:
        case unary::op::RLA:
        case unary::op::RLCA:
            return {Reg::a, get(Reg::a), carry};

        default:
            return CPU::get_unary_args_cb(addr_mode);
    }

}

CPU::unary_op_args CPU::get_unary_args_non_cb() {
    
    // INC/DEC B[0] - 0x04/0x05, INC/DEC C[1] - 0x0C/0x0D
    // INC/DEC D[2] - 0x14/0x15, INC/DEC E[3] - 0x1C/0x1D
    // INC/DEC H[4] - 0x24/0x25, INC/DEC L[5] - 0x2C/0x2D
    // INC/DEC(HL)[6]-0x34/0x35, INC/DEC A[7] - 0x3C/0x3D
    bool carry = get_carry();
    auto row = fetched[0] >> 4;
    int column = -1;
    auto reg_mask = fetched[0] & 0xF;

    if (reg_mask == 0x4 || reg_mask == 0x5)
        column = 0;
    if (reg_mask == 0xC || reg_mask == 0xD)
        column = 1;

    byte reg_calc = 2 * row + column;

    if (reg_calc == 6) {
        word address = get(DReg::hl);
        return {address, read(address), carry};
    }

    Reg reg = static_cast<Reg>(reg_calc);
    return {reg, get(reg), carry};
}

CPU::unary_op_args CPU::get_unary_args_cb(unary::addr_modes addressing_mode) {
    bool carry = get_carry();
    switch (addressing_mode) {
        case unary::addr_modes::REG: // INC r8
        {
            Reg reg_index = static_cast<Reg>(fetched[1] & 0x7);
            return {reg_index, get(reg_index), carry};
        }
        case unary::addr_modes::MEM : //INC [HL]
        {
            word address = get(DReg::hl);
            return {address, read(address), carry};
        }
        default: {
            std::cout << " Fall Through Unary\n";
            return {1, 1, false};
        }
    }
}

void CPU::DI_r16(int op_id) {
    //INC BC - 0x03
    //INC DE - 0x13
    //INC HL - 0x23
    //INC SP - 0x33
    int bitmask = (fetched[0] >> 4);
    DReg reg_index = static_cast<DReg>(2 * bitmask);
    word value = get(reg_index);

    if (op_id == unary::op::INC)
        value++;

    if (op_id == unary::op::DEC)
        value--;

    set(reg_index, value);
}

FlagStatusResponseWrapper CPU::INC(unary_op_args &arg) {
    byte value = arg.value;
    value++;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = (value & 0xF) == 0;

    return {{
                    {Flag::z, z_bit},
                    {Flag::n, n_bit},
                    {Flag::h, h_bit}
            }, value};

}

FlagStatusResponseWrapper CPU::DEC(unary_op_args &arg) {
    byte value = arg.value;
    value--;

    bool z_bit = value == 0;
    bool n_bit = true;
    bool h_bit = (value & 0xF) == 0xF;

    return {{
                    {Flag::z, z_bit},
                    {Flag::n, n_bit},
                    {Flag::h, h_bit}
            }, value};
}

FlagStatusResponseWrapper CPU::SWAP(unary_op_args &arg) {
    byte value = arg.value, lo, hi;
    lo = value & 0xF;
    hi = (value >> 4) & 0xF;
    value = (lo << 4) + hi;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = false;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), value};
}

FlagStatusResponseWrapper CPU::RL_helper(unary_op_args &arg, byte lsb) {
    auto value = arg.value;

    // b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_6 b_5 b_4 b_3 b_2 b_1 b_0 lsb
    // carry -> b_7
    word temp = value;
    (temp <<= 1) |= lsb;
    value = temp & 0xFF;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = (temp >> 8) == 1;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), value};
}

FlagStatusResponseWrapper CPU::RR_helper(unary_op_args &arg, byte msb) {
    auto value = arg.value;
    byte carry = value & 1;

    // b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> msb b_7 b_6 b_5 b_4 b_3 b_2 b_1
    // carry -> b_0
    word temp = value;
    (temp >>= 1) |= (msb << 7);
    value = temp & 0xFF;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = carry == 1;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), value};
}

// msb/lsb is 0
FlagStatusResponseWrapper CPU::SLA(unary_op_args &arg) {
    return RL_helper(arg, 0);
}

FlagStatusResponseWrapper CPU::SRL(unary_op_args &arg) {
    return RR_helper(arg, 0);
}

// msb/lsb is carry bit
FlagStatusResponseWrapper CPU::RL(unary_op_args &arg) {
    return RL_helper(arg, arg.carry);
}

FlagStatusResponseWrapper CPU::RR(unary_op_args &arg) {
    return RR_helper(arg, arg.carry);
}

// lsb/msb is bit-7/bit-0 - Rotate bits
// RLC: b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_6 b_5 b_4 b_3 b_2 b_1 b_0 b_7
// RRC: b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_0 b_7 b_6 b_5 b_4 b_3 b_2 b_1
FlagStatusResponseWrapper CPU::RLC(unary_op_args &arg) {
    return RL_helper(arg, arg.value >> 7);
}

FlagStatusResponseWrapper CPU::RRC(unary_op_args &arg) {
    return RR_helper(arg, arg.value & 0x1);
}

//  b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_7 b_7 b_6 b_5 b_4 b_3 b_2 b_1
FlagStatusResponseWrapper CPU::SRA(unary_op_args &arg) {
    return RR_helper(arg, arg.value >> 7);
}

// special variant of RL/RR for register A
FlagStatusResponseWrapper CPU::RLA(unary_op_args &arg) {
    auto temp_result = RL(arg);
    temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::z, false));
    return temp_result;
}

FlagStatusResponseWrapper CPU::RRA(unary_op_args &arg) {
    auto temp_result = RR(arg);
    temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::z, false));
    return temp_result;
}

// special variant of RLC/RRC for register A
FlagStatusResponseWrapper CPU::RLCA(unary_op_args &arg) {
    auto temp_result = RLC(arg);
    temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::z, false));
    return temp_result;
}

FlagStatusResponseWrapper CPU::RRCA(unary_op_args &arg) {
    auto temp_result = RRC(arg);
    temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::z, false));
    return temp_result;
}
