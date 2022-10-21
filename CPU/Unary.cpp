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

Unary::op_args::op_args() {
    location = static_cast<word>(0);
    value = 0;
}

void Unary::Un_16(word &value, int op_id) {
    auto inc = [](word &a) { a++; };
    auto dec = [](word &a) { a--; };
    std::function<void(word &)> Un[2] = {inc, dec};
    Un[op_id](value);
}

void Unary::dispatch(vector<Flag_Status> &flags, CPU *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {

    if (addr_mode == unary::addr_modes::REG_16) {
        int bitmask = (bytes_fetched[0] >> 4);
        DReg reg_index = static_cast<DReg>(2 * bitmask);
        word value = cpu->get(reg_index);
        Un_16(value, op_id);
        cpu->set(reg_index, value);
        return;
    }

    auto args = Unary::get_args(cpu, bytes_fetched, addr_mode);

    if (op_id == unary::op::INC || op_id == unary::op::DEC) {
        byte reg_calc = 2 * (bytes_fetched[0] >> 4) + ((bytes_fetched[0] & 0x8) > 0);

        if (reg_calc == 6) {
            auto address = cpu->get(DReg::hl);
            args.location = address;
            args.value = cpu->read(address);
        } else {
            Reg reg_index = static_cast<Reg>(reg_calc);
            args.location = reg_index;
            args.value = cpu->get(reg_index);
        }
    }

    if (op_id == unary::op::RRA || op_id == unary::op::RRCA || op_id == unary::op::RLA || op_id == unary::op::RLCA) {
        args.location = Reg::a;
        args.value = cpu->get(Reg::a);
    }

    byte value = Unary::op_codes[op_id](flags, args);

    if (args.location.index() == 0) {
        Reg reg_index = std::get<0>(args.location);
        cpu->set(reg_index, value);
    }

    if (args.location.index() == 1) {
        word address = std::get<1>(args.location);
        cpu->write(address, value);
    }

}

Unary::op_args Unary::get_args(CPU *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Unary::op_args result;
    switch (addressing_mode) {
        case unary::addr_modes::REG: // INC r8
        {
            Reg reg_index = static_cast<Reg>(bytes_fetched[1] & 0x7);
            result.location = reg_index;
            result.value = cpu->get(reg_index);
            break;
        }
        case unary::addr_modes::MEM : //INC [HL]
        {
            word address = cpu->get(DReg::hl);
            result.location = address;
            result.value = cpu->read(address);
            break;
        }
        default:
            break;
    }
    return result;
}

byte Unary::INC(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte value = arg.value;
    value++;

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, ((value & 0xF) == 0)));

    return value;
}

byte Unary::DEC(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte value = arg.value;
    value--;

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, true));
    flags.emplace_back(set(Flag::h, ((value & 0xF) == 0xF)));

    return value;
}

byte Unary::SWAP(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte value = arg.value, lo, hi;
    lo = value & 0xF;
    value = value >> 4;
    hi = value & 0xF;
    value = hi + (lo << 4);

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, false));

    return value;
}

byte Unary::RL_helper(vector<Flag_Status> &flags, Unary::op_args arg, byte lsb) {
    auto value = arg.value;
    word temp = value;
    (temp *= 2) += lsb;
    value = temp & 0xFF;

    flags.emplace_back(set(Flag::z, (value == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, ((temp >> 8) > 0)));

    return value;
}

byte Unary::SLA(vector<Flag_Status> &flags, Unary::op_args arg) {
    return RL_helper(flags, arg, 0);
}

byte Unary::RL(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte carry = flags.size();
    return RL_helper(flags, arg, carry);
}

byte Unary::RLA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RL(flags, arg);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RLC(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = arg.value;
    byte msb = value >> 7;
    return RL_helper(flags, arg, msb);
}

byte Unary::RLCA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RLC(flags, arg);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RR_helper(vector<Flag_Status> &flags, Unary::op_args arg, byte lsb) {
    auto value = arg.value;
    word temp = value;
    byte carry = temp & 0x01;
    (temp /= 2) += (lsb << 7);

    flags.emplace_back(set(Flag::z, (temp == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, false));
    flags.emplace_back(set(Flag::c, (carry == 1)));

    return temp;
}

byte Unary::SRL(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto res = RR_helper(flags, arg, 0);
    return res;
}

byte Unary::RR(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte carry = flags.size();
    return RR_helper(flags, arg, carry);
}

byte Unary::RRA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RR(flags, arg);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

byte Unary::RRC(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = arg.value;
    byte lsb = value & 0x1;
    return RR_helper(flags, arg, lsb);
}

byte Unary::SRA(vector<Flag_Status> &flags, Unary::op_args arg) {
    byte lsb = arg.value >> 7;
    return RR_helper(flags, arg, lsb);
}

byte Unary::RRCA(vector<Flag_Status> &flags, Unary::op_args arg) {
    auto value = RRC(flags, arg);
    flags.emplace_back(set(Flag::z, false));
    return value;
}

