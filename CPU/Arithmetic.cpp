//
// Created by drake on 2/9/22.
//

#include "Arithmetic.hpp"
#include "Cpu.hpp"

const std::function<byte(vector<Flag_Status> &, Arithmetic::op_args)> Arithmetic::op_codes[8] = {Arithmetic::ADD,
                                                                                                 Arithmetic::ADC,
                                                                                                 Arithmetic::AND,
                                                                                                 Arithmetic::CP,
                                                                                                 Arithmetic::OR,
                                                                                                 Arithmetic::SBC,
                                                                                                 Arithmetic::SUB,
                                                                                                 Arithmetic::XOR};

Arithmetic::op_args::op_args(byte src_value, byte value) : src_value(src_value), value(value) {};

void Arithmetic::dispatch(vector<Flag_Status> &flags, CPU *cpu, int op_id, vector<byte> &bytes_fetched,
                          arithmetic::addr_modes addr_mode) {

    if (addr_mode == arithmetic::addr_modes::ADD_16) {

        word src = cpu->get(DReg::hl);

        // 0x09 - ADD HL,BC
        // 0x19 - ADD HL,DE
        // 0x29 - ADD HL,HL
        // 0x39 - ADD HL,SP
        int bitmask = (bytes_fetched[0] >> 4);
        DReg reg_index = static_cast<DReg>(2 * bitmask);
        word addend = cpu->get(reg_index);

        word result = ADD_16(flags, src, addend);
        cpu->set(DReg::hl, result);

        return;
    }

    if (addr_mode == arithmetic::addr_modes::SP) { //ADD SP,e8
        word src = cpu->get(DReg::sp);
        auto offset = bytes_fetched[1];

        word result = ADD_TO_SP(flags, src, offset);
        cpu->set(DReg::sp, result);
        return;
    }

    auto args = Arithmetic::get_args(cpu, bytes_fetched, addr_mode);
    byte result = Arithmetic::op_codes[op_id](flags, args);
    cpu->set(Reg::a, result);
}

Arithmetic::op_args Arithmetic::get_args(CPU *cpu, vector<byte> bytes_fetched, arithmetic::addr_modes addressing_mode) {
    byte src_value = cpu->get(Reg::a);
    byte value = 0;

    switch (addressing_mode) {
        case arithmetic::addr_modes::IMM : //ADC A,u8
        {
            value = bytes_fetched[1];
            break;
        }
        case arithmetic::addr_modes::REG : //ADC A,r8
        {
            Reg reg_index = static_cast<Reg>(bytes_fetched[0] & 0x7);
            value = cpu->get(reg_index);
            break;
        }
        case arithmetic::addr_modes::MEM :// ADC A,[HL]
        {
            value = cpu->read(cpu->get(DReg::hl));
            break;
        }
        default:
            break;
    }

    return {src_value, value};
}


word Arithmetic::ADD_16(vector<Flag_Status> &flags, word src, word addend) {
    word temp = src + addend;

    bool n_bit = false;
    bool h_bit = (src & 0xFFF) + (addend & 0xFFF) > 0xFFF;
    bool c_bit = (src & 0xFFFF) + (addend & 0xFFFF) > 0xFFFF;

    flags = {
            {Flag::n, n_bit},
            {Flag::h, h_bit},
            {Flag::c, c_bit}
    };
    return temp;
}

word Arithmetic::ADD_TO_SP(vector<Flag_Status> &flags, word src_value, byte offset) {
    word signed_offset = get_signed_offset(offset);
    word temp = src_value + signed_offset;

    bool z_bit = false;
    bool n_bit = false;
    bool h_bit = (src_value & 0xF) + (signed_offset & 0xF) > 0xF;
    bool c_bit = (src_value & 0xFF) + (signed_offset & 0xFF) > 0xFF;


    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return temp;
}

byte Arithmetic::ADC(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte temp, carry = flags.size(), old_value = arg.value;
    flags.clear();

    //Reduce to ADD case by incrementing the value to be added by carry. Is Valid if arg.value + carry <= 0xFF(i.e. sum gives a valid 8-bit byte without wraparound)
    arg.value += carry;
    temp = Arithmetic::ADD(flags, arg);

    //Wraparound case
    if ((old_value & 0xF) + (carry & 0xF) > 0xF)
        flags.emplace_back(set(Flag::h, true));

    if (old_value + carry > 0xFF)
        flags.emplace_back(set(Flag::c, true));

    return temp;
}

byte Arithmetic::SBC(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte temp, carry = flags.size(), old_value = arg.value;
    flags.clear();

    //Reduce to SUB case by incrementing the value to be subtracted by carry. Is Valid if arg.value + carry <= 255(i.e. sum gives a valid 8-bit byte without wraparound)
    arg.value += carry;
    temp = Arithmetic::SUB(flags, arg);

    //Wraparound case
    if (old_value + carry > 0xFF)
        flags.emplace_back(set(Flag::c, true));

    if ((old_value & 0xF) + (carry & 0xF) > 0xF)
        flags.emplace_back(set(Flag::h, true));

    return temp;
}


byte Arithmetic::ADD(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte value = arg.value, src = arg.src_value;
    byte temp = src + value;

    bool z_bit = temp == 0;
    bool n_bit = false;
    bool h_bit = (src & 0xF) + (value & 0xF) > 0xF;
    bool c_bit = (src & 0xFF) + (value & 0xFF) > 0xFF;

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return temp;
}

byte Arithmetic::AND(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte src = arg.src_value, value = arg.value;
    byte temp = src & value;

    bool z_bit = temp == 0;
    bool n_bit = false;
    bool h_bit = true;
    bool c_bit = false;

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return temp;
}

byte Arithmetic::OR(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte src = arg.src_value, value = arg.value;
    byte temp = src | value;

    bool z_bit = temp == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = false;

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return temp;
}

byte Arithmetic::CP(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte src = arg.src_value, value = arg.value;

    bool z_bit = src - value == 0;
    bool n_bit = true;
    bool h_bit = (value & 0xF) > (src & 0xF);
    bool c_bit = (value & 0xFF) > (src & 0xFF);

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return src;
}

byte Arithmetic::SUB(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte src, value = arg.value;
    src = Arithmetic::CP(flags, arg);
    byte temp = src - value;

    //All flags set by CP instruction
    return temp;
}

byte Arithmetic::XOR(vector<Flag_Status> &flags, Arithmetic::op_args arg) {
    byte src = arg.src_value, value = arg.value;
    byte temp = src ^ value;

    bool z_bit = temp == 0;
    bool n_bit = false;
    bool h_bit = false;
    bool c_bit = false;

    flags = batch_fill({z_bit, n_bit, h_bit, c_bit});
    return temp;
}