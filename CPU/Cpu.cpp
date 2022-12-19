//
// Created by drake on 27/8/22.
//

#include "Cpu.hpp"
#include "Instructions.hpp"
#include "Arithmetic.hpp"
#include "Unary.hpp"
#include "Bit_Operations.hpp"
#include "Load.hpp"
#include "Store.hpp"
#include "Jump_and_Stack.hpp"
#include "Misc.hpp"
#include "../MMU/Mmu.hpp"

CPU::CPU(MMU *mmu) {
    reg_mapper = {};

    cycles_to_increment = 0;
    mem_ptr = mmu;
    flags.reserve(10);

    SP = 0xFFFE;
    PC = 0x0100;
    set(DReg::af, 0x01B0);
    set(DReg::hl, 0x014D);
    set(DReg::bc, 0x0013);
    set(DReg::de, 0x00D8);
}

//void CPU::halt(bool status) { //TODO implement correct halt logic
//    Halt = status;
//}

int CPU::run_instruction_cycle() {
    cycles_to_increment = 0;
    flags.clear();
    byte index = read(PC);
    Instructions curr = Instruction_List[index];
    if (index == 0xCB)
        curr = Prefix_List[read(PC + 1)];

    vector<byte> fetched = fetch(curr);
    decode_and_execute(std::move(fetched), curr);
    set_flags(flags);
    return cycles_to_increment;
}

void CPU::push(byte to_push) {
    write(--SP, to_push);
}

byte CPU::pop() {
    return read(SP++);
}

byte CPU::read(word address) {
    return mem_ptr->read(address);
}

void CPU::write(word address, byte value) {
    mem_ptr->write(address, value);
}

byte CPU::get(Reg reg_index) {
    auto index = static_cast<int>(reg_index);
    return reg_mapper[index];
}

word CPU::get(DReg reg_index) {
    switch (reg_index) {
        case DReg::pc:
            return PC;
        case DReg::sp:
            return SP;
        default: {
            auto index = static_cast<int>(reg_index);
            word value = (reg_mapper[index] << 8) + reg_mapper[index + 1];
            return value;
        }
    }
}

void CPU::set(Reg reg_index, byte value) {
    auto index = static_cast<int>(reg_index);
    reg_mapper[index] = value;
}

void CPU::set(DReg reg_index, word val) {
    auto index = static_cast<int>(reg_index);
    switch (reg_index) {
        case DReg::pc:
            PC = val;
            break;
        case DReg::sp:
            SP = val;
            break;
        default:
            reg_mapper[index + 1] = val & 0xFF;
            val >>= 8;
            reg_mapper[index] = val & 0xFF;
    }
}

void CPU::set_flags(vector<Flag_Status> &flag_array) {
    byte F = reg_mapper[8];
    for (auto flag_c: flag_array) {
        Flag bit = flag_c.bit;
        bool set = flag_c.status;
        auto bit_pos = static_cast<int>(bit);
        byte bitmask = 0xFF - (1 << bit_pos);
        if (set)
            F |= (1 << bit_pos);
        else
            F &= bitmask;
    }
    reg_mapper[8] = F;
}

vector<byte> CPU::fetch(Instructions &instruction_data) {
    vector<byte> fetched;

    auto bytes_to_fetch = instruction_data.bytes_to_fetch;
    cycles_to_increment = instruction_data.cycles;

    for (int i = 0; i < bytes_to_fetch; i++) {
        fetched.push_back(read(PC++));
    }

    byte flag_data = get(Reg::f);

    //Hack: If carry flag is set,push it onto Flag Status for usage by XXC instructions
    if (flag_data & (1 << Flag::c)) {
        flags.emplace_back(Flag_Status(Flag::c, true));
    }
    return fetched;
}

void CPU::decode_and_execute(vector<byte> fetched, Instructions &instruction_data) {
    auto Type = instruction_data.instr_type;
    auto op_id = instruction_data.op_id;
    auto addr_mode = instruction_data.addr_mode;

    switch (Type) {
        case Type::ARITHMETIC: {
            Arithmetic::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case Type::UNARY: {
            Unary::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case Type::BIT_OP: {
            Bit_Operations::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case Type::LOAD: {
            Load::dispatch(this, op_id, fetched, addr_mode);
            return;
        }
        case Type::STORE: {
            Store::dispatch(this, op_id, fetched, addr_mode);
            return;
        }
        case Type::JUMP: {
            Jump::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case Type::MISC: {
            Misc::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        default:
            return;
    }
}