//
// Created by drake on 27/8/22.
//

#ifndef GGBE_CPU_HPP
#define GGBE_CPU_HPP

#include "Utility.hpp"
#include "Timer.hpp"
#include "Instructions.hpp"
#include <fstream>
#include <sstream>

class MMU;

class Console;

class CPU {
private:
    word SP, PC;
    std::array<byte, 9> reg_mapper{};
    vector<Flag_Status> flags;

    Console *console;
    MMU *mem_ptr;
    std::ofstream write_file;
    vector<byte> boot_data;

    bool IME;
    byte interrupt_data;

    Timer timer;
    Instructions current_instruction;
    int dma_cycles;
public:

    bool halt_bug;

    bool is_boot;

    bool start_logging;

    std::vector<bool> keys_pressed;

    int counter;

    bool halt_mode;

    int interrupt_buffer;

private:
    void debug();

    void set_interrupt_master_flag();

    void handle_interrupts();

    std::string string_write();

public:
    explicit CPU(Console *console);

    void run_boot_rom();

    void push(byte to_push);

    byte pop();

    byte read(word address, bool tick_components = true);

    void write(word address, byte value, bool tick_components = true);

    byte get(Reg reg_index);

    word get(DReg reg_index);

    void set(Reg reg_index, byte value);

    void set(DReg reg_index, word value);

    void set_pc(word address, bool flush = true);

    void set_flags();

    vector<byte> fetch();

    void decode_and_execute(vector<byte> &fetched);

    void run_instruction_cycle();

    void tick_components();

    bool get_current_interrupt_status();
};


#endif //GGBE_CPU_HPP