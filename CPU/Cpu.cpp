//
// Created by drake on 27/8/22.
//

#include "Cpu.hpp"
#include "../Console/Console.hpp"

Instructions::Instructions() {
    Type = 0;
    op_id = 0;
    addr_mode = 0;
    bytes_to_fetch = 0;
    cycles = 0;
}

Instructions::Instructions(int Type, int op_id, int addr_mode, int bytes_to_fetch, int cycles) {
    this->Type = Type;
    this->op_id = op_id;
    this->addr_mode = addr_mode;
    this->bytes_to_fetch = bytes_to_fetch;
    this->cycles = cycles;
}


Cpu::Cpu(Console *game) {
    reg_mapper = array<byte, 9>{0};
    auto zero = static_cast<word>(0);
    SP = zero;
    PC = zero;
    this->game = game;
    Instruction_List = array<Instructions, 2>{
            Instructions(0, 0, 0, 2, 1),
            Instructions(0, 1, 1, 1, 1)
    };
}

void Cpu::loop() {
    vector<Flag_Status> flags;
    Instructions curr = Instruction_List[PC];
    vector<byte> fetched = fetch(flags, curr);
    decode_and_execute(flags, std::move(fetched), curr);
    set_flags(flags);
}

void Cpu::push(byte to_push) {
    stack.push(to_push);
}

byte Cpu::pop() {
    byte value = stack.top();
    stack.pop();
    return value;
}

byte Cpu::read(word address) {
    return game->read(address);
}

void Cpu::write(word address, byte value) {
    game->write(address, value);
}

byte Cpu::get(Reg reg_index) {
    auto index = static_cast<int>(reg_index);
    return reg_mapper[index];
}

word Cpu::get(DReg reg_index) {
    switch (reg_index) {
        case DReg::pc:
            return PC;
        case DReg::sp:
            return SP;
        default:
            auto index = static_cast<int>(reg_index);
            word value = (reg_mapper[index] << 8) + reg_mapper[index + 1];
            return value;
    }
}

void Cpu::set(Reg reg_index, byte value) {
    auto index = static_cast<int>(reg_index);
    reg_mapper[index] = value;
}

void Cpu::set(DReg reg_index, word val) {
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

void Cpu::set_flags(vector<Flag_Status> &flag_array) {
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

vector<byte> Cpu::fetch(vector<Flag_Status> &flags, Instructions &instruction_data) {
    vector<byte> fetched;

    auto bytes_to_fetch = instruction_data.bytes_to_fetch;
    auto cycles = instruction_data.cycles;

    for (int i = 0; i < bytes_to_fetch; i++) {
        fetched.push_back(read(PC++));
    }

    byte flag_data = get(Reg::f);
    if ((flag_data & (1 << Flag::c)) > 0) {
        flags.emplace_back(Flag_Status(Flag::c, true));
    }
    std::cout << cycles;
    return fetched;
}

void Cpu::decode_and_execute(vector<Flag_Status> &flags, vector<byte> fetched, Instructions &instruction_data) {
    auto Type = instruction_data.Type;
    auto op_id = instruction_data.op_id;
    auto addr_mode = instruction_data.addr_mode;

    switch (Type) {
        case 0: {
            Arithmetic::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case 1: {
            Unary::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case 2: {
            Bit_Operations::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case 3: {
            Load::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        case 4: {
            Store::dispatch(flags, this, op_id, fetched, addr_mode);
            return;
        }
        default:
            return;
    }
}