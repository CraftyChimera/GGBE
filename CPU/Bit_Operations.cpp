//
// Created by drake on 11/9/22.
//

#include "Bit_Operations.hpp"
#include "Cpu.hpp"

const std::function<byte(vector<Flag_Status> &, Bit_Operations::op_args)> Bit_Operations::op_codes[3] = {
        Bit_Operations::BIT,
        Bit_Operations::RES,
        Bit_Operations::SET};

Bit_Operations::op_args::op_args() : test_bit(0), value(0), location(Reg::a) {}

void Bit_Operations::dispatch(vector<Flag_Status> &flags, Cpu *cpu, int op_id, vector<byte> &fetched, int addr_mode) {
    auto args = Bit_Operations::get_args(cpu, fetched, addr_mode);
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
        case bit_op::addr_modes::REG : //BIT u3,r8
        {
            byte data_value = bytes_fetched[1];
            Reg reg_index = static_cast<Reg>(data_value & 0x7);

            result.value = cpu->get(reg_index);
            result.test_bit = (data_value >> 3) & 0x7;
            result.location = reg_index;

            break;
        }
        case bit_op::addr_modes::MEM :  // BIT u3,[HL]
        {
            word address = cpu->get(DReg::hl);

            result.value = cpu->read(address);
            result.test_bit = (bytes_fetched[1] >> 3) & 0x7;
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

word Bit_Operations::RES([[maybe_unused]] vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    byte bit = arg.test_bit, value = arg.value;
    byte bitmask = 0xFF - (1 << bit);
    return value & bitmask;
}

word Bit_Operations::SET([[maybe_unused]]vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    byte bit = arg.test_bit, value = arg.value;
    byte bitmask = (1 << bit);
    return value | bitmask;
}
