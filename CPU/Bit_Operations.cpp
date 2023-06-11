//
// Created by drake on 11/9/22.
//

#include "Bit_Operations.hpp"
#include "Cpu.hpp"

const std::function<byte(vector<Flag_Status> &, Bit_Operations::op_args)> Bit_Operations::op_codes[3] = {
        Bit_Operations::BIT,
        Bit_Operations::RES,
        Bit_Operations::SET
};

Bit_Operations::op_args::op_args() : test_bit(0), value(0), location(Reg::a) {}

Bit_Operations::op_args::op_args(byte test_bit, byte value, Reg reg) : test_bit(test_bit), value(value),
                                                                       location(reg) {}

Bit_Operations::op_args::op_args(byte test_bit, byte value, word address) : test_bit(test_bit), value(value),
                                                                            location(address) {}

void Bit_Operations::dispatch(vector<Flag_Status> &flags, CPU *cpu, int op_id, vector<byte> &fetched,
                              bit_op::addr_modes addr_mode) {
    auto args = Bit_Operations::get_args(cpu, fetched, addr_mode);

    word value = Bit_Operations::op_codes[op_id](flags, args);

    // BIT instructions don't need to write to address/register
    if (op_id == bit_op::op::BIT)
        return;

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

Bit_Operations::op_args
Bit_Operations::get_args(CPU *cpu, vector<byte> &bytes_fetched, bit_op::addr_modes addressing_mode) {
    byte temp = bytes_fetched.at(1);
    byte test_bit = (temp >> 3) & 0x7;

    switch (addressing_mode) {
        //All the instructions of this class are CB prefixed
        case bit_op::addr_modes::REG : //BIT u3,r8
        {
            Reg reg_index = static_cast<Reg>(temp & 0x7);
            return {test_bit, cpu->get(reg_index), reg_index};
        }

        case bit_op::addr_modes::MEM :  // BIT u3,[HL]
        {
            word address = cpu->get(DReg::hl);
            return {test_bit, cpu->read(address), address};
        }
        default: {
            std::cout << "Fall-through bit-op\n";
            return {1, 1, 1};
        }
    }
}

byte Bit_Operations::BIT(vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    byte bit = arg.test_bit, value = arg.value;

    bool z_bit = (value & (1 << bit)) == 0;
    bool n_bit = false;
    bool h_bit = true;

    flags = {
            {Flag::z, z_bit},
            {Flag::n, n_bit},
            {Flag::h, h_bit}
    };

    return value;
}

byte Bit_Operations::RES(vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    flags.clear();
    byte bit = arg.test_bit, value = arg.value;

    byte bitmask = 0xFF - (1 << bit);
    return value & bitmask;
}

byte Bit_Operations::SET(vector<Flag_Status> &flags, Bit_Operations::op_args arg) {
    flags.clear();
    byte bit = arg.test_bit, value = arg.value;

    byte bitmask = (1 << bit);
    return value | bitmask;
}
