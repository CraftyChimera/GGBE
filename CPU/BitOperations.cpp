//
// Created by drake on 11/9/22.
//

#include "Cpu.hpp"

CPU::bit_operations_op_args::bit_operations_op_args(byte test_bit, byte value, Reg reg) : test_bit(test_bit),
                                                                                          value(value),
                                                                                          location(reg) {}

CPU::bit_operations_op_args::bit_operations_op_args(byte test_bit, byte value, word address) : test_bit(test_bit),
                                                                                               value(value),
                                                                                               location(address) {}

void CPU::bit_operations_dispatch(int op_id, bit_op::addr_modes addr_mode) {
    auto args = bit_operations_get_args(addr_mode);
    auto result = bit_operations_op_codes[op_id](args);
    auto value = result.result_byte;
    auto new_flags = result.new_flag_status;
    set_flags(new_flags);

    // BIT instructions don't need to write to address/register
    if (op_id == bit_op::op::BIT)
        return;

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

CPU::bit_operations_op_args CPU::bit_operations_get_args(bit_op::addr_modes addressing_mode) {
    byte temp = fetched.at(1);
    byte test_bit = (temp >> 3) & 0x7;

    switch (addressing_mode) {
        //All the instructions of this class are CB prefixed
        case bit_op::addr_modes::REG : //BIT u3,r8
        {
            Reg reg_index = static_cast<Reg>(temp & 0x7);
            return {test_bit, get(reg_index), reg_index};
        }

        case bit_op::addr_modes::MEM :  // BIT u3,[HL]
        {
            word address = get(DReg::hl);
            return {test_bit, read(address), address};
        }
        default: {
            std::cout << "Fall-through bit-op\n";
            return {1, 1, 1};
        }
    }
}

FlagStatusResponseWrapper CPU::BIT(bit_operations_op_args &arg) {
    byte bit = arg.test_bit, value = arg.value;

    bool z_bit = (value & (1 << bit)) == 0;
    bool n_bit = false;
    bool h_bit = true;

    return {{
                    {Flag::z, z_bit},
                    {Flag::n, n_bit},
                    {Flag::h, h_bit}
            }, value};
}

FlagStatusResponseWrapper CPU::RES(bit_operations_op_args &arg) {
    byte bit = arg.test_bit, value = arg.value;

    byte bitmask = 0xFF - (1 << bit);
    return {{}, static_cast<byte>(value & bitmask)};
}

FlagStatusResponseWrapper CPU::SET(bit_operations_op_args &arg) {
    byte bit = arg.test_bit, value = arg.value;

    byte bitmask = (1 << bit);
    return {{}, static_cast<byte>(value | bitmask)};
}
