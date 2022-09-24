//
// Created by drake on 11/9/22.
//

#include "Bit_Operations.hpp"
#include "Cpu.hpp"

Bit_Operations::op_args::op_args() : test_bit(0), value(0), location(Reg::a) {}

void
Bit_Operations::dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &bytes_fetched, int addr_mode) {

    auto args = Bit_Operations::get_args(cpu, bytes_fetched, addr_mode);
    word value = Bit_Operations::op_codes[op_id](flags, args);

    if (args.location.index() == 0) {
        Reg reg_index = std::get<0>(args.location);
        cpu->set(reg_index, value);
    }

    if (args.location.index() == 1) {
        word address = std::get<1>(args.location);
        cpu->write(address, value);
    }

}

Bit_Operations::op_args Bit_Operations::get_args(Cpu *cpu, vector<byte> &bytes_fetched, int addressing_mode) {
    Bit_Operations::op_args result;
    switch (addressing_mode) {
        case 0: {
            byte data_value = bytes_fetched[0];
            Reg reg_index = static_cast<Reg>(data_value & 0x7);
            data_value >>= 3;

            result.value = cpu->get(reg_index);
            result.test_bit = data_value & 0x7;
            result.location = reg_index;

            break;
        }
        case 1: {
            word address = cpu->get(DReg::hl);

            result.value = cpu->read(address);
            result.test_bit = (bytes_fetched[0] >> 3) & 0x7;
            result.location = address;

            break;
        }
        default:
            break;
    }
    return result;
}

word Bit_Operations::BIT(vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    byte bit = arg.test_bit, value = arg.value;
    flags.emplace_back(set(Flag::z, ((value & (1 << bit)) == 0)));
    flags.emplace_back(set(Flag::n, false));
    flags.emplace_back(set(Flag::h, true));
    return arg.value;
}

word Bit_Operations::RES(vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    byte bit = arg.test_bit, value = arg.value;
    byte bitmask = 0xFF - (1 << bit);
    return value & bitmask;
}

word Bit_Operations::SET(vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    byte bit = arg.test_bit, value = arg.value;
    byte bitmask = (1 << bit);
    return value | bitmask;
}
