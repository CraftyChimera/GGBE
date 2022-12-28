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
#include "Mmu.hpp"
#include <sstream>
#include "../Base/Parser.hpp"

CPU::CPU(MMU *mmu) {
    reg_mapper = {};

    cycles_to_increment = 0;
    mem_ptr = mmu;
    flags.reserve(10);
    counter = 0;
    SP = 0x0000;
    PC = 0x0000;
    is_boot = true;
    write_file.open("roms/Gameboy-logs/test.txt");
    boot_data = read_file("roms/boot.gb");
}

//void CPU::halt(bool status) { //TODO implement correct halt logic
//    Halt = status;
//}

void CPU::run_boot_rom() {
    while (PC < 0x1000) {
        run_instruction_cycle();
    }
    is_boot = false;
    write(0xFF44, 0x90);
}

std::string byte_to_string(byte x) {
    std::stringstream stream;
    stream << std::hex << static_cast<int>(x);
    std::string result(stream.str());
    result = std::string(2 - result.size(), '0') + result;

    for (auto &y: result) {
        if (y - 'a' >= 0 && y - 'a' <= 'z' - 'a')
            y -= 32;
    }

    return result;
}

std::string word_to_string(word x) {
    std::stringstream stream;
    stream << std::hex << static_cast<int>(x);
    std::string result(stream.str());
    result = std::string(4 - result.size(), '0') + result;

    for (auto &y: result) {
        if (y - 'a' >= 0 && y - 'a' <= 'z' - 'a')
            y -= 32;
    }

    return result;
}

std::string string_write(CPU *cpu) {

    std::string _8bit_to_write = "A: " + byte_to_string(cpu->get(Reg::a)) +
                                 " F: " + byte_to_string(cpu->get(Reg::f)) +
                                 " B: " + byte_to_string(cpu->get(Reg::b)) +
                                 " C: " + byte_to_string(cpu->get(Reg::c)) +
                                 " D: " + byte_to_string(cpu->get(Reg::d)) +
                                 " E: " + byte_to_string(cpu->get(Reg::e)) +
                                 " H: " + byte_to_string(cpu->get(Reg::h)) +
                                 " L: " + byte_to_string(cpu->get(Reg::l));

    std::string _16bit_to_write = " SP: " + word_to_string(cpu->get(DReg::sp)) +
                                  " PC: 00:" + word_to_string(cpu->get(DReg::pc));

    word PC = cpu->get(DReg::pc);
    std::string mem_bits = " (" + byte_to_string(cpu->read(PC)) +
                           " " + byte_to_string(cpu->read(PC + 1)) +
                           " " + byte_to_string(cpu->read(PC + 2)) +
                           " " + byte_to_string(cpu->read(PC + 3)) +
                           ")";

    return _8bit_to_write + _16bit_to_write + mem_bits;
}

int CPU::run_instruction_cycle() {
    cycles_to_increment = 0;
    flags.clear();
    byte index = read(PC);
    Instructions curr = Instruction_List[index];
    if (index == 0xCB)
        curr = Prefix_List[read(PC + 1)];


    std::string to_write = string_write(this);

    static int max_c = 223892;
    if (PC >= 0x0100) {
        write_file << to_write << "\n";
        counter++;

        if (counter == max_c) {
            std::cout << "Done";
            write_file.close();
            exit(1);
        }
    }

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
    if (address >= 0x0100)
        return mem_ptr->read(address);

    if (is_boot)
        return boot_data.at(address);
    else
        return mem_ptr->read(address);
}

void CPU::write(word address, byte value) {
    if (address == 0xFF44) //Read-Only register for CPU
        return;

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
            return;
        case DReg::sp:
            SP = val;
            return;
        default:
            reg_mapper[index + 1] = val & 0xFF;
            val >>= 8;
            reg_mapper[index] = val & 0xFF;
    }
}

void CPU::set_flags(vector<Flag_Status> &flag_array) {
    byte F = get(Reg::f);
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
    set(Reg::f, F);
}

vector<byte> CPU::fetch(Instructions &instruction_data) {
    vector<byte> fetched;

    auto bytes_to_fetch = instruction_data.bytes_to_fetch;
    cycles_to_increment = instruction_data.cycles;

    for (int i = 0; i < bytes_to_fetch; i++) {
        fetched.push_back(read(PC++));
    }

    byte flag_data = get(Reg::f);

    //Hack: If carry flag is set,push it onto Flag Status for usage by XXC instructions. Problematic for Instructions that directly modify F register
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
            Arithmetic::dispatch(flags, this, op_id, fetched, std::get<arithmetic::addr_modes>(addr_mode));
            return;
        }
        case Type::UNARY: {
            Unary::dispatch(flags, this, op_id, fetched, std::get<unary::addr_modes>(addr_mode));
            return;
        }
        case Type::BIT_OP: {
            Bit_Operations::dispatch(flags, this, op_id, fetched, std::get<bit_op::addr_modes>(addr_mode));
            return;
        }
        case Type::MISC: {
            Misc::dispatch(flags, this, op_id);
            return;
        }

        case Type::LOAD: {
            flags.clear();
            Load::dispatch(this, op_id, fetched, std::get<load::addr_modes>(addr_mode));
            return;
        }
        case Type::STORE: {
            flags.clear();
            Store::dispatch(this, op_id, fetched, std::get<store::addr_modes>(addr_mode));
            return;
        }
        case Type::JUMP: {
            flags.clear();
            Jump::dispatch(this, op_id, fetched, std::get<jump_stack::addr_modes>(addr_mode));
            return;
        }
        default:
            return;
    }
}