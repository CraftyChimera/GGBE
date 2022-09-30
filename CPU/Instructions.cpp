//
// Created by drake on 25/9/22.
//

#include "Instructions.hpp"

Instructions::Instructions() noexcept {
    Type = 0;
    op_id = 0;
    addr_mode = 0;
    bytes_to_fetch = 0;
    cycles = 0;
}

Instructions::Instructions(int Type, int op_id, int addr_mode, int bytes_to_fetch, int cycles) noexcept {
    this->Type = Type;
    this->op_id = op_id;
    this->addr_mode = addr_mode;
    this->bytes_to_fetch = bytes_to_fetch;
    this->cycles = cycles;
}

const Instructions Instruction_List[256] =
        {
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM_16, 3, 3),
                Instructions(Type::STORE, store::op::NOTHING, store::addr_modes::MEM_r16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::UNARY, unary::op::RLCA, unary::addr_modes::REG, 1, 1),

                Instructions(Type::STORE, store::op::NOTHING, store::addr_modes::SP, 3, 5),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::ADD_16, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_r16, 1, 2),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::UNARY, unary::op::RRCA, unary::addr_modes::REG, 1, 1),


                Instructions(Type::MISC, misc::op::STOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM_16, 3, 3),
                Instructions(Type::STORE, store::op::NOTHING, store::addr_modes::MEM_r16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::UNARY, unary::op::RLA, unary::addr_modes::REG, 1, 1),

                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::REL, 2, 3),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::ADD_16, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_r16, 1, 2),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::UNARY, unary::op::RRA, unary::addr_modes::REG, 1, 1),


                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::REL, 2, 2),//2-3
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM_16, 3, 3),
                Instructions(Type::STORE, store::op::INCREMENT, store::addr_modes::MEM_DI, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::MISC, misc::op::DAA, misc::addr_modes::NOTHING, 1, 1),

                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::REL, 2, 2),//2-3
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::ADD_16, 1, 2),
                Instructions(Type::LOAD, load::op::INCREMENT, load::addr_modes::MEM_DI, 1, 2),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::MISC, misc::op::CPL, misc::addr_modes::NOTHING, 1, 1),


                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::REL, 2, 2),//2-3
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM_16, 3, 3),
                Instructions(Type::STORE, store::op::DECREMENT, store::addr_modes::MEM_DI, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::MEM, 1, 3),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::MEM, 1, 3),
                Instructions(Type::STORE, store::op::NOTHING, store::addr_modes::IMM, 2, 3),
                Instructions(Type::MISC, misc::op::SCF, misc::addr_modes::NOTHING, 1, 1),

                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::REL, 2, 2),//2-3
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::ADD_16, 1, 2),
                Instructions(Type::LOAD, load::op::DECREMENT, load::addr_modes::MEM_DI, 1, 2),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG_16, 1, 2),
                Instructions(Type::UNARY, unary::op::INC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::UNARY, unary::op::DEC, unary::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::IMM, 2, 2),
                Instructions(Type::MISC, misc::op::CCF, misc::addr_modes::NOTHING, 1, 1),


                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),

                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),


                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),

                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),


                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),

                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),


                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 2),
                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 2),
                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 2),
                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 2),
                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 2),
                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 2),
                Instructions(Type::MISC, misc::op::HALT, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::STORE, load::op::NOTHING, load::addr_modes::REG, 1, 1),

                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_HL, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REG, 1, 1),


                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::REG, 1, 1),

                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::REG, 1, 1),


                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::REG, 1, 1),

                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::REG, 1, 1),


                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::REG, 1, 1),

                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::REG, 1, 1),


                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::REG, 1, 1),

                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::MEM, 1, 2),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::REG, 1, 1),


                Instructions(Type::JUMP, jump_stack::op::RETC, jump_stack::addr_modes::IMM, 1, 2), //2-5
                Instructions(Type::JUMP, jump_stack::op::POP, jump_stack::addr_modes::PUSH_POP, 1, 3),
                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::IMM, 3, 3), //3-4
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::IMM, 3, 4),
                Instructions(Type::JUMP, jump_stack::op::CALLC, jump_stack::addr_modes::IMM, 3, 3), //3-6
                Instructions(Type::JUMP, jump_stack::op::PUSH, jump_stack::addr_modes::PUSH_POP, 1, 4),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),

                Instructions(Type::JUMP, jump_stack::op::RETC, jump_stack::addr_modes::IMM, 1, 2), //2-5
                Instructions(Type::JUMP, jump_stack::op::RET, jump_stack::addr_modes::IMM, 1, 4),
                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::IMM, 3, 3), //3-4
                Instructions(-1, -1, -1, 2, 1), //TODO prefix CB
                Instructions(Type::JUMP, jump_stack::op::CALLC, jump_stack::addr_modes::IMM, 3, 3), //3-6
                Instructions(Type::JUMP, jump_stack::op::CALL, jump_stack::addr_modes::IMM, 3, 6),
                Instructions(Type::ARITHMETIC, arithmetic::op::ADC, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),


                Instructions(Type::JUMP, jump_stack::op::RETC, jump_stack::addr_modes::IMM, 1, 2), //2-5
                Instructions(Type::JUMP, jump_stack::op::POP, jump_stack::addr_modes::PUSH_POP, 1, 3),
                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::IMM, 3, 3), //3-4
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::JUMP, jump_stack::op::CALLC, jump_stack::addr_modes::IMM, 3, 3), //3-6
                Instructions(Type::JUMP, jump_stack::op::PUSH, jump_stack::addr_modes::PUSH_POP, 1, 4),
                Instructions(Type::ARITHMETIC, arithmetic::op::SUB, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),

                Instructions(Type::JUMP, jump_stack::op::RETC, jump_stack::addr_modes::IMM, 1, 2), //2-5
                Instructions(Type::JUMP, jump_stack::op::RETI, jump_stack::addr_modes::IMM, 1, 4),
                Instructions(Type::JUMP, jump_stack::op::JPC, jump_stack::addr_modes::IMM, 3, 3), //3-4
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::JUMP, jump_stack::op::CALLC, jump_stack::addr_modes::IMM, 3, 3), //3-6
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::SBC, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),


                Instructions(Type::STORE, store::op::HIGH_IMM, store::addr_modes::MEM_IMM, 2, 3),
                Instructions(Type::JUMP, jump_stack::op::POP, jump_stack::addr_modes::PUSH_POP, 1, 3),
                Instructions(Type::STORE, store::op::HIGH_C, store::addr_modes::MEM_IMM, 1, 2),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::JUMP, jump_stack::op::PUSH, jump_stack::addr_modes::PUSH_POP, 1, 4),
                Instructions(Type::ARITHMETIC, arithmetic::op::AND, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),

                Instructions(Type::ARITHMETIC, arithmetic::op::ADD, arithmetic::addr_modes::SP, 2, 4),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::MEM, 1, 4),
                Instructions(Type::STORE, store::op::NOTHING, store::addr_modes::MEM_IMM, 3, 4),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::XOR, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),


                Instructions(Type::LOAD, load::op::HIGH_IMM, load::addr_modes::MEM_IMM, 2, 3),
                Instructions(Type::JUMP, jump_stack::op::POP, jump_stack::addr_modes::PUSH_POP, 1, 3),
                Instructions(Type::LOAD, load::op::HIGH_C, load::addr_modes::MEM_IMM, 1, 2),
                Instructions(Type::MISC, misc::op::DI, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::JUMP, jump_stack::op::PUSH, jump_stack::addr_modes::PUSH_POP, 1, 4),
                Instructions(Type::ARITHMETIC, arithmetic::op::OR, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),

                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::REL_16, 2, 3),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::SP, 1, 2),
                Instructions(Type::LOAD, load::op::NOTHING, load::addr_modes::MEM_IMM, 3, 4),
                Instructions(Type::MISC, misc::op::EI, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::MISC, misc::op::NOP, misc::addr_modes::NOTHING, 1, 1),
                Instructions(Type::ARITHMETIC, arithmetic::op::CP, arithmetic::addr_modes::IMM, 2, 2),
                Instructions(Type::JUMP, jump_stack::op::JP, jump_stack::addr_modes::RST, 1, 4),
        };

const Instructions Prefix_List[256] = {
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::RLC, unary::addr_modes::REG, 2, 2),

        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::RRC, unary::addr_modes::REG, 2, 2),


        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::RL, unary::addr_modes::REG, 2, 2),

        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::RR, unary::addr_modes::REG, 2, 2),


        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::SLA, unary::addr_modes::REG, 2, 2),

        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::SRA, unary::addr_modes::REG, 2, 2),


        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::SWAP, unary::addr_modes::REG, 2, 2),

        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::MEM, 2, 4),
        Instructions(Type::UNARY, unary::op::SRL, unary::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::BIT, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::RES, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),


        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),

        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::MEM, 2, 4),
        Instructions(Type::BIT_OP, bit_op::op::SET, bit_op::addr_modes::REG, 2, 2),
};