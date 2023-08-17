//
// Created by drake on 2/9/22.
//

#include "Cpu.hpp"

CPU::arithmetic_op_args::arithmetic_op_args(byte src_value, byte value, bool carry) : src_value(src_value),
                                                                                      value(value), carry(carry) {}

void CPU::arithmetic_dispatch(int op_id, arithmetic::addr_modes &addr_mode) {
    if (arithmetic_check_and_deal_with_edge_cases(addr_mode))
        return;

    auto args = arithmetic_get_args(addr_mode);
    auto result = arithmetic_op_codes[op_id](args);
    arithmetic_set_result(result);
}

CPU::arithmetic_op_args CPU::arithmetic_get_args(arithmetic::addr_modes &addr_mode) {
    byte src_value = get(Reg::a);
    bool carry = get_carry();
    byte value = 0;

    switch (addr_mode) {
        case arithmetic::addr_modes::IMM : //ADC A,u8
        {
            value = fetched[1];
            break;
        }
        case arithmetic::addr_modes::REG : //ADC A,r8
        {
            Reg reg_index = static_cast<Reg>(fetched[0] & 0x7);
            value = get(reg_index);
            break;
        }
        case arithmetic::addr_modes::MEM :// ADC A,[HL]
        {
            value = read(get(DReg::hl));
            break;
        }
        default:
            break;
    }

    return {src_value, value, carry};
}

bool CPU::arithmetic_check_and_deal_with_edge_cases(arithmetic::addr_modes &addr_mode) {
    if (addr_mode == arithmetic::addr_modes::ADD_16) {
        tick_components();
        word src = get(DReg::hl);

        // 0x09 - ADD HL,BC; 0x19 - ADD HL,DE; 0x29 - ADD HL,HL; 0x39 - ADD HL,SP
        int bitmask = (fetched[0] >> 4);
        DReg reg_index = static_cast<DReg>(2 * bitmask);
        word addend = get(reg_index);

        word result = ADD_16(src, addend);
        set(DReg::hl, result);
        return true;
    }

    if (addr_mode == arithmetic::addr_modes::SP) { // ADD SP,e8
        word src = get(DReg::sp);
        auto offset = fetched[1];

        word result = ADD_TO_SP(src, offset);
        tick_components();
        tick_components();
        set(DReg::sp, result);
        return true;
    }
    return false;
}

void CPU::arithmetic_set_result(FlagStatusResponseWrapper &result) {
    byte value = result.result_byte;
    auto new_flags = result.new_flag_status;

    set_flags(new_flags);
    set(Reg::a, value);
}


word CPU::ADD_16(word src, word addend) {
    word temp = src + addend;

    bool n_bit = false;
    bool h_bit = (src & 0xFFF) + (addend & 0xFFF) > 0xFFF;
    bool c_bit = (src & 0xFFFF) + (addend & 0xFFFF) > 0xFFFF;

    auto new_flags = vector<FlagStatus>(
            {
                    {Flag::n, n_bit},
                    {Flag::h, h_bit},
                    {Flag::c, c_bit}
            });
    set_flags(new_flags);
    return temp;
}

word CPU::ADD_TO_SP(word src_value, byte offset) {
    word signed_offset = get_signed_offset(offset);
    word temp = src_value + signed_offset;

    bool z_bit = false;
    bool n_bit = false;
    bool h_bit = (src_value & 0xF) + (signed_offset & 0xF) > 0xF;
    bool c_bit = (src_value & 0xFF) + (signed_offset & 0xFF) > 0xFF;


    auto new_flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    set_flags(new_flags);
    return temp;
}

FlagStatusResponseWrapper CPU::ADC(arithmetic_op_args &arg) {
    byte carry = arg.carry, old_value = arg.value;

    //Reduce to ADD case by incrementing the value to be added by carry. Is Valid if arg.value + carry <= 0xFF(i.e. sum gives a valid 8-bit FlagStatusResponseWrapper without wraparound)
    arg.value += carry;
    auto temp_result = CPU::ADD(arg);

    //Wraparound case
    if ((old_value & 0xF) + (carry & 0xF) > 0xF)
        temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::h, true));

    if (old_value + carry > 0xFF)
        temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::c, true));

    return temp_result;
}

FlagStatusResponseWrapper CPU::SBC(arithmetic_op_args &arg) {
    byte carry = arg.carry, old_value = arg.value;

    //Reduce to SUB case by incrementing the value to be subtracted by carry. Is Valid if arg.value + carry <= 255(i.e. sum gives a valid 8-bit byte without wraparound)
    arg.value += carry;
    auto temp_result = CPU::SUB(arg);

    //Wraparound case
    if (old_value + carry > 0xFF)
        temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::c, true));

    if ((old_value & 0xF) + (carry & 0xF) > 0xF)
        temp_result.new_flag_status.emplace_back(add_flag_status_change(Flag::h, true));

    return temp_result;
}


FlagStatusResponseWrapper CPU::ADD(arithmetic_op_args &arg) {
    byte value = arg.value, src = arg.src_value;
    byte temp = src + value;

    bool z_bit = temp == 0;
    bool n_bit = false;
    bool h_bit = (src & 0xF) + (value & 0xF) > 0xF;
    bool c_bit = (src & 0xFF) + (value & 0xFF) > 0xFF;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), temp};
}

FlagStatusResponseWrapper CPU::AND(arithmetic_op_args &arg) {
    byte src = arg.src_value, value = arg.value;
    value = src & value;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = true;
    bool c_bit = false;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), value};
}

FlagStatusResponseWrapper CPU::OR(arithmetic_op_args &arg) {
    byte src = arg.src_value, value = arg.value;
    value = src | value;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = false;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), value};
}

FlagStatusResponseWrapper CPU::CP(arithmetic_op_args &arg) {
    byte src = arg.src_value, value = arg.value;

    bool z_bit = src - value == 0;
    bool n_bit = true;
    bool h_bit = (value & 0xF) > (src & 0xF);
    bool c_bit = (value & 0xFF) > (src & 0xFF);

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), src};
}

FlagStatusResponseWrapper CPU::SUB(arithmetic_op_args &arg) {
    // All flags set by CP instruction
    byte src = arg.src_value, value = arg.value;
    auto temp_result = CPU::CP(arg);
    temp_result.result_byte = src - value;
    return temp_result;
}

FlagStatusResponseWrapper CPU::XOR(arithmetic_op_args &arg) {
    byte src = arg.src_value, value = arg.value;
    value = src ^ value;

    bool z_bit = value == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = false;

    return {batch_fill({z_bit, n_bit, h_bit, c_bit}), value};
}

