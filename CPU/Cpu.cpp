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
#include "../Base/Parser.hpp"

CPU::CPU(MMU *mmu) : timer(mmu) {
    cycles_to_increment = 0;
    mem_ptr = mmu;
    flags.reserve(10);
    reg_mapper = {};
    SP = 0x0000;
    PC = 0x0000;
    is_boot = true;
    boot_data = read_file("roms/boot.gb");
    IME = false;
    interrupt_buffer = 0;
    halt_mode = false;
    write_file.open("roms/Logs.txt");
    keys_pressed.assign(8, false);
    counter = 0;
    dma_cycles = 0;
}

int CPU::run_boot_rom() {
    if (PC >= 0x0100) {
        is_boot = false;
        return 0;
    }
    return run_instruction_cycle();
}

// Check for DMA -> Check for HALT mode -> Tick timer -> Check for Interrupts -> Run Fetch-Decode-Execute cycle
int CPU::run_instruction_cycle() {
    // Check for DMA
    if (mem_ptr->dma_started) {
        mem_ptr->dma_started = false;
        dma_cycles = 160;
    }

    if (dma_cycles > 0) {
        dma_cycles--;
        timer.tick(1);
        return 1;
    }

    // Check for HALT MODE
    auto interrupt_flags = read(if_address);
    auto interrupt_enable = read(ie_address);
    auto interrupt_data = interrupt_flags & interrupt_enable;

    if (halt_mode) {
        timer.tick(1);
        if (interrupt_data == 0)
            return 1;
        halt_mode = false;
    }

    // Fetch instruction data and tick the timer appropriately
    byte index = read(PC);
    Instructions curr = Instruction_List[index];
    if (index == 0xCB)
        curr = Prefix_List[read(PC + 1)];

    cycles_to_increment = curr.cycles;
    timer.tick(cycles_to_increment);

    // Check for interrupts
    interrupt_flags = read(if_address);
    interrupt_data = interrupt_flags & interrupt_enable;

    if (IME && (interrupt_data != 0)) {
        handle_interrupts(interrupt_data);
        write_file << " INT RAISED \n";
        return cycles_to_increment;
    }

    //Run Fetch-Decode-Execute cycle
    //debug();
    flags.clear();
    vector<byte> fetched = fetch(curr);
    decode_and_execute(std::move(fetched), curr);
    set_flags(flags);
    set_interrupt_master_flag();

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

    if (address == joypad_reg_address) {
        (value >>= 4) <<= 4;
        value = value | 0xC0;
        byte key_data = 0;
        if ((value & (1 << 5)) == 0) {
            for (int i = 0; i < 4; i++) {
                key_data = key_data | (keys_pressed[i + 4] << i);
            }
        }
        if ((value & (1 << 4)) == 0) {
            for (int i = 0; i < 4; i++)
                key_data = key_data | (keys_pressed[i] << i);
        }
        key_data ^= 0x0F;
        value |= key_data;
    }

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
    byte flag_data = get(Reg::f);

    for (int i = 0; i < bytes_to_fetch; i++) {
        fetched.push_back(read(PC++));
    }

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

    if (Type == Type::LOAD || Type == Type::STORE || Type == Type::JUMP)
        flags.clear();

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
            Load::dispatch(this, op_id, fetched, std::get<load::addr_modes>(addr_mode));
            return;
        }

        case Type::STORE: {
            Store::dispatch(this, op_id, fetched, std::get<store::addr_modes>(addr_mode));
            return;
        }

        case Type::JUMP: {
            byte cycles_ticked_so_far = cycles_to_increment;
            Jump::dispatch(this, op_id, fetched, std::get<jump_stack::addr_modes>(addr_mode));
            timer.tick(cycles_to_increment - cycles_ticked_so_far);
            return;
        }

        default:
            return;
    }
}

void CPU::set_interrupt_master_flag() {
    if (interrupt_buffer > 0) {
        if (interrupt_buffer == 1)
            IME = true;

        interrupt_buffer--;
    }

    if (interrupt_buffer == -1) {
        IME = false;
        interrupt_buffer = 0;
    }
}

void CPU::handle_interrupts(byte interrupt_data) {
    const vector<word> interrupt_vectors = {0x0040, 0x0048, 0x0050, 0x0058, 0x0060};
    for (auto bit_pos = 0; bit_pos < 5; bit_pos++) {
        if ((interrupt_data & (1 << bit_pos)) == 0)
            continue;

        IME = false;
        interrupt_data -= (1 << bit_pos);
        write(if_address, interrupt_data);
        cycles_to_increment = 5;
        auto jump_address = interrupt_vectors.at(bit_pos);
        Jump::op_args args;
        args.jump_address = jump_address;
        Jump::CALL(this, args);
        return;
    }
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

std::string CPU::string_write() {

    std::string _8bit_to_write = " A: " + byte_to_string(get(Reg::a)) +
                                 " F: " + byte_to_string(get(Reg::f)) +
                                 " B: " + byte_to_string(get(Reg::b)) +
                                 " C: " + byte_to_string(get(Reg::c)) +
                                 " D: " + byte_to_string(get(Reg::d)) +
                                 " E: " + byte_to_string(get(Reg::e)) +
                                 " H: " + byte_to_string(get(Reg::h)) +
                                 " L: " + byte_to_string(get(Reg::l));

    std::string _16bit_to_write = " SP: " + word_to_string(get(DReg::sp)) +
                                  " PC: 00:" + word_to_string(get(DReg::pc));

    auto ie_reg = read(ie_address);
    auto if_reg = read(if_address);
    auto stat_reg = read(lcd_stat_address);

    std::string interrupt =
            " IME: " + byte_to_string(IME) + " IE: " + byte_to_string(ie_reg) + " IF: " + byte_to_string(if_reg) +
            " STAT: " +
            byte_to_string(stat_reg);

    auto tima_reg = read(tima_address);
    auto div_reg = read(div_address);
    auto tma_reg = read(tma_address);
    auto tac_reg = read(tac_address);

    std::string timer_internal = "RST: " + byte_to_string(timer.cycles_to_irq) +
                                 " INC: " + word_to_string(cycles_to_increment);

    std::string timer_stats = "TIMA: " + byte_to_string(tima_reg) +
                              " TMA: " + byte_to_string(tma_reg) +
                              " DIV: " + byte_to_string(div_reg) +
                              " TAC: " + byte_to_string(tac_reg);

    std::string gpu_status = " LY: " + byte_to_string(read(ly_address));
    std::string mem_bits = " (" + byte_to_string(read(PC)) +
                           " " + byte_to_string(read(PC + 1)) +
                           " " + byte_to_string(read(PC + 2)) +
                           " " + byte_to_string(read(PC + 3)) +
                           ")";

    return timer_internal + " " + timer_stats + _8bit_to_write + _16bit_to_write + mem_bits;
}

void CPU::debug() {
    if (is_boot)
        return;
    std::string to_write = string_write();
    write_file << counter << " " << to_write << "\n";
    counter++;
}