//
// Created by drake on 27/8/22.
//

#include "../Base/Console.hpp"
#include "../Base/Parser.hpp"
#include "Cpu.hpp"
#include "Instructions.hpp"

CPU::CPU(Console *base) : reg_mapper{}, console(base), ptr_to_bus(&console->bus), timer(ptr_to_bus) {
    SP = 0x0000;
    PC = 0x0000;
    fetched = {};
    is_boot = true;

    if (!ptr_to_bus->is_cgb)
        boot_data = read_file("BootRoms/boot.gb");
    else {
        boot_data = read_file("BootRoms/boot.gbc", is_boot = true);
        if (boot_data.empty())
            set_boot_values();
    }

    IME = false;
    interrupt_data = 0;
    interrupt_buffer = 0;
    halt_mode = false;
    keys_pressed.assign(8, false);
    counter = 0;
    current_instruction = {};
    start_logging = false;
    halt_bug = false;
    dma_cycles = 0;
}

void CPU::set_boot_values() {
    is_boot = false;
    PC = 0x0100;
    SP = 0xFFFE;
    set(Reg::a, 0x11);
    set(Reg::f, 0x80);
    set(Reg::b, 0x00);
    set(Reg::c, 0x00);
    set(Reg::d, 0xFF);
    set(Reg::e, 0x56);
    set(Reg::h, 0x00);
    set(Reg::l, 0x0D);
    write(tac_address, 0xF8);
    write(if_address, 0xE1);
    write(lcd_control_address, 0x91);
    write(vbk_address, 0xFF);
    write(svbk_address, 0xFF);
}

std::string byte_to_string(byte x) {
    std::stringstream stream;
    stream << std::hex << static_cast<int>(x);
    std::string result(stream.str());
    result = std::string(2 - result.size(), '0') + result;

    for (auto &y: result)
        if (y - 'a' >= 0 && y - 'a' <= 'z' - 'a')
            y -= 32;

    return result;
}

std::string word_to_string(word x) {
    std::stringstream stream;
    stream << std::hex << static_cast<int>(x);
    std::string result(stream.str());
    result = std::string(4 - result.size(), '0') + result;

    for (auto &y: result)
        if (y - 'a' >= 0 && y - 'a' <= 'z' - 'a')
            y -= 32;

    return result;
}

void CPU::run_instruction_cycle() {
    if (halt_mode) {
        while (!is_interrupt_pending())
            tick_components();

        halt_mode = false;
        if (IME)
            handle_interrupts();
    }

    byte opcode = read(PC, false);
    current_instruction = (opcode == 0xCB) ? Prefix_List[read(PC + 1, false)]
                                           : Instruction_List[opcode];

    fetch();
    if (IME && is_interrupt_pending()) {
        PC -= current_instruction.bytes_to_fetch;
        current_instruction = {};
        handle_interrupts();
    } else {
        decode_and_execute();
    }
    set_interrupt_master_flag();
}

void CPU::push(byte to_push) {
    write(--SP, to_push);
}

byte CPU::pop() {
    return read(SP++);
}

byte CPU::read(word address, bool tick) {
    if (tick)
        tick_components();

    if (is_boot && ptr_to_bus->is_cgb)
        if (address >= 0x0200 && address < 0x0900)
            return boot_data.at(address);

    if (address >= 0x0100)
        return ptr_to_bus->read(address);

    if (is_boot)
        return boot_data.at(address);
    else
        return ptr_to_bus->read(address);
}

void CPU::write(word address, byte value, bool tick) {
    if (tick)
        tick_components();

    if (address == 0xFF50) {
        if (value != 0)
            is_boot = false;
    }

    if (address == 0xFF44) //Read-Only register for CPU
        return;

    if (address == dma_address)
        dma_cycles = 160;

    if (address == joypad_reg_address) {
        (value >>= 4) <<= 4;
        value = value | 0xC0;
        byte key_data = 0;
        if ((value & (1 << 5)) == 0) {
            for (int i = 0; i < 4; i++)
                key_data = key_data | (keys_pressed[i + 4] << i);
        }
        if ((value & (1 << 4)) == 0) {
            for (int i = 0; i < 4; i++)
                key_data = key_data | (keys_pressed[i] << i);
        }
        key_data ^= 0x0F;
        value |= key_data;
    }

    ptr_to_bus->write(address, value);
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

void CPU::set_pc(word address, bool flush) {
    if (flush)
        tick_components();

    set(DReg::pc, address);
}

void CPU::set_flags(vector<FlagStatus> &new_flags) {
    byte F = get(Reg::f);
    for (auto flag_c: new_flags) {
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

void CPU::fetch() {
    //if (start_logging)
    //    debug();
    auto bytes_to_fetch = current_instruction.bytes_to_fetch;
    fetched = {};

    for (int i = 0; i < bytes_to_fetch; i++) {
        fetched.push_back(read(PC++));
        if (halt_bug) {
            halt_bug = false;
            PC--;
        }
    }
}

void CPU::decode_and_execute() {
    auto Type = current_instruction.instr_type;
    auto op_id = current_instruction.op_id;
    auto addr_mode = current_instruction.addr_mode;

    switch (Type) {
        case Type::ARITHMETIC:
            arithmetic_dispatch(op_id, std::get<arithmetic::addr_modes>(addr_mode));
            return;

        case Type::UNARY:
            unary_dispatch(op_id, std::get<unary::addr_modes>(addr_mode));
            return;

        case Type::BIT_OP:
            bit_operations_dispatch(op_id, std::get<bit_op::addr_modes>(addr_mode));
            return;

        case Type::LOAD:
            load_dispatch(op_id, std::get<load::addr_modes>(addr_mode));
            return;

        case Type::STORE:
            store_dispatch(op_id, std::get<store::addr_modes>(addr_mode));
            return;

        case Type::JUMP:
            jump_stack_dispatch(op_id, std::get<jump_stack::addr_modes>(addr_mode));
            return;

        case Type::MISC:
            misc_dispatch(op_id, std::get<misc::addr_modes>(addr_mode));
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

void CPU::handle_interrupts() {
    const vector<word> interrupt_vectors = {0x0040, 0x0048, 0x0050, 0x0058, 0x0060};
    for (auto bit_pos = 0; bit_pos < 5; bit_pos++) {
        if ((interrupt_data & (1 << bit_pos)) == 0)
            continue;

        /*if (start_logging)
            write_file << std::dec << counter++ << " INT RAISED: " << (interrupt_data) << "\n";
        */

        IME = false;
        interrupt_data -= (1 << bit_pos);
        write(if_address, interrupt_data, false);

        tick_components();
        tick_components();

        word current_pc = get(DReg::pc);
        byte hi = current_pc >> 8, lo = current_pc & 0xFF;
        push(hi);
        push(lo);

        auto jump_address = interrupt_vectors.at(bit_pos);
        set_pc(jump_address, false);
        return;
    }
}

void CPU::tick_components() {
    timer.tick();
    console->tick();

    if (dma_cycles > 0 && --dma_cycles == 0)
        ptr_to_bus->oam_dma_active = false;
}

std::string CPU::get_string_to_write() {

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

    auto ie_reg = read(ie_address, false) & 0x1F;
    auto if_reg = read(if_address, false) & 0x1F;
    auto stat_reg = read(lcd_stat_address, false);

    std::string interrupt =
            " IME: " + byte_to_string(IME) + " IE: " + byte_to_string(ie_reg) + " IF: " + byte_to_string(if_reg);

    auto tima_reg = read(tima_address, false);
    auto div_reg = read(div_address, false);
    auto tma_reg = read(tma_address, false);
    auto tac_reg = read(tac_address, false);

    std::string timer_stats = " TIMA: " + byte_to_string(tima_reg) +
                              " TMA: " + byte_to_string(tma_reg) +
                              " DIV: " + byte_to_string(div_reg) +
                              " TAC: " + byte_to_string(tac_reg);

    std::string gpu_status = " LY: " + byte_to_string(read(ly_address, false));
    std::string mem_bits = " (" + byte_to_string(read(PC, false)) +
                           " " + byte_to_string(read(PC + 1, false)) +
                           " " + byte_to_string(read(PC + 2, false)) +
                           " " + byte_to_string(read(PC + 3, false)) +
                           ")";

    std::string ppu =
            " LCD_C: " + byte_to_string(read(lcd_control_address, false)) + " STAT: " + byte_to_string(stat_reg);

    return timer_stats + interrupt + _8bit_to_write + _16bit_to_write + mem_bits;
}

[[maybe_unused]] void CPU::debug() {
    if (is_boot)
        return;

    std::string to_write = get_string_to_write();
    write_file << std::dec << counter << " " << to_write << "\n";
    counter++;
}

bool CPU::is_interrupt_pending() {
    auto interrupt_flags = read(if_address, false) & 0x1F;
    auto interrupt_enable = read(ie_address, false) & 0x1F;

    interrupt_data = interrupt_flags & interrupt_enable;
    return (interrupt_data != 0);
}

bool CPU::get_carry() {
    return get(Reg::f) & (1 << Flag::c);
}