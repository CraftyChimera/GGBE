//
// Created by drake on 4/9/22.
//

#include "Unary.hpp"
#include "Cpu.hpp"

const std::function<byte(vector<Flag_Status> &, Unary::op_args)> Unary::op_codes[14] = {Unary::INC, Unary::DEC,
                                                                                        Unary::SWAP,
                                                                                        Unary::RL, Unary::RLA,
                                                                                        Unary::RLC, Unary::RLCA,
                                                                                        Unary::RR, Unary::RRA,
                                                                                        Unary::RRC,
                                                                                        Unary::RRCA, Unary::SLA,
                                                                                        Unary::SRL, Unary::SRA};

Unary::op_args::op_args() : location(Reg::a), value(0) {}

Unary::op_args::op_args(Reg reg, byte value) : location(reg), value(value) {}

Unary::op_args::op_args(word address, byte value) : location(address), value(value) {}

void Unary::dispatch(vector<Flag_Status> &flags, CPU *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {

    if (addr_mode == unary::addr_modes::REG_16) {
        DI_r16(cpu, bytes_fetched, op_id);
        return;
    }

    auto args = Unary::get_args(cpu, op_id, bytes_fetched, addr_mode);
    byte value = Unary::op_codes[op_id](flags, args);

    switch (args.location.index()) {
        case 0: {
            Reg reg_index = std::get<0>(args.location);
            cpu->set(reg_index, value);
            break;
        }
        case 1: {
            word address = std::get<1>(args.location);
            cpu->write(address, value);
            break;
        }
    }
}

Unary::op_args Unary::get_args(CPU *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {
    switch (op_id) {
        case unary::op::INC:
        case unary::op::DEC:
            return Unary::get_args_non_cb(cpu, bytes_fetched);

        case unary::op::RRA:
        case unary::op::RRCA:
        case unary::op::RLA:
        case unary::op::RLCA:
            return {Reg::a, cpu->get(Reg::a)};

        default:
            return Unary::get_args_cb(cpu, bytes_fetched, addr_mode);
    }

}

Unary::op_args Unary::get_args_non_cb(CPU *cpu, vector<byte> &bytes_fetched) {
    // INC/DEC B[0] - 0x04/0x05, INC/DEC C[1] - 0x0C/0x0D
    // INC/DEC D[2] - 0x14/0x15, INC/DEC E[3] - 0x1C/0x1D
    // INC/DEC H[4] - 0x24/0x25, INC/DEC L[5] - 0x2C/0x2D
    // INC/DEC(HL)[6]-0x34/0x35, INC/DEC A[7] - 0x3C/0x3D
    auto row = bytes_fetched[0] >> 4;

    int column = -1;
    switch (bytes_fetched[0] & 0xF) {
        case 0x4:
        case 0x5:
            column = 0;
            break;
        case 0xC:
        case 0xD:
            column = 1;
            break;
        default: {
            std::cout << "Fall through INC/DEC \n";
        }
    }

    byte reg_calc = 2 * row + column;

    if (reg_calc == 6) {
        word address = cpu->get(DReg::hl);
        return {address, cpu->read(address)};
    }

    Reg reg = static_cast<Reg>(reg_calc);
    return {reg, cpu->get(reg)};
}

Unary::op_args Unary::get_args_cb(CPU *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    switch (addressing_mode) {
        case unary::addr_modes::REG: // INC r8
        {
            Reg reg_index = static_cast<Reg>(bytes_fetched[1] & 0x7);
            return {reg_index, cpu->get(reg_index)};
        }
        case unary::addr_modes::MEM : //INC [HL]
        {
            word address = cpu->get(DReg::hl);
            return {address, cpu->read(address)};
        }
        default: {
            std::cout << " Fall Through Unary\n";
            return {1, 1};
        }
    }
}

void Unary::DI_r16(CPU *cpu, vector<byte> &bytes_fetched, int op_id) {
    //INC BC - 0x03
    //INC DE - 0x13
    //INC HL - 0x23
    //INC SP - 0x33
    int bitmask = (bytes_fetched[0] >> 4);
    DReg reg_index = static_cast<DReg>(2 * bitmask);
    word value = cpu->get(reg_index);

    if (op_id == unary::op::INC)
        value++;

    if (op_id == unary::op::DEC)
        value--;

    cpu->set(reg_index, value);
}

byte Unary::INC(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte value = arg.value;
    value++;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = (value & 0xF) == 0;

    flags = {
            {Flag::z, z_bit},
            {Flag::n, n_bit},
            {Flag::h, h_bit}
    };

    return value;
}

byte Unary::DEC(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte value = arg.value;
    value--;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = (value & 0xF) == 0;

    flags = {
            {Flag::z, z_bit},
            {Flag::n, n_bit},
            {Flag::h, h_bit}
    };

    return value;
}

byte Unary::SWAP(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte value = arg.value, lo, hi;
    lo = value & 0xF;
    hi = (value >> 4) & 0xF;
    value = (lo << 4) + hi;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = false;

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});

    return value;
}

byte RL_helper(vector<Flag_Status> &flags, Unary::op_args arg, byte lsb) {
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

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return value;
}

byte RR_helper(vector<Flag_Status> &flags, Unary::op_args arg, byte msb) {
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

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return value;
}

// msb/lsb is 0
byte Unary::SLA(vector<Flag_Status> &flags, Unary::op_args arg) {
    return RL_helper(flags, arg, 0);
}

byte Unary::SRL(vector<Flag_Status> &flags, Unary::op_args arg) {
    return RR_helper(flags, arg, 0);
}

// msb/lsb is carry bit
byte Unary::RL(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte carry = flags.size();
    flags.clear();

    return RL_helper(flags, arg, carry);
}

byte Unary::RR(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte carry = flags.size();
    flags.clear();

    return RR_helper(flags, arg, carry);
}

// lsb/msb is bit-7/bit-0 - Rotate bits
// RLC: b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_6 b_5 b_4 b_3 b_2 b_1 b_0 b_7
// RRC: b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_0 b_7 b_6 b_5 b_4 b_3 b_2 b_1
byte Unary::RLC(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte lsb = arg.value >> 7;
    return RL_helper(flags, arg, lsb);
}

byte Unary::RRC(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = arg.value;
    byte msb = value & 0x1;
    return RR_helper(flags, arg, msb);
}

//  b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0 -> b_7 b_7 b_6 b_5 b_4 b_3 b_2 b_1
byte Unary::SRA(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte msb = arg.value >> 7;
    return RR_helper(flags, arg, msb);
}


//TODO: Potentially dangerous clever code

// special variant of RL/RR for register A
byte Unary::RLA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RL(flags, arg);

    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RRA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RR(flags, arg);

    flags.emplace_back(set(Flag::z, false));
    return value;
}

// special variant of RLC/RRC for register A
byte Unary::RLCA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RLC(flags, arg);

    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RRCA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RRC(flags, arg);
    flags.emplace_back(set(Flag::z, false));
    return value;
}
