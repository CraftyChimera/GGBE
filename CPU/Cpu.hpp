//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CPU_HPP
#define DMGB_CPU_HPP

#include "Utility.hpp"
#include "Timer.hpp"
#include <fstream>
#include <sstream>

class Instructions;

class MMU;

class CPU {
private:
    word SP, PC;
    std::array<byte, 9> reg_mapper{};
    vector<Flag_Status> flags;
    MMU *mem_ptr;
    std::ofstream write_file;
    vector<byte> boot_data;
    bool IME;
    Timer timer;
    int dma_cycles;

public:

    bool is_boot;

    bool start_logging;

    std::vector<bool> keys_pressed;

    int counter;

    bool halt_mode;

    int cycles_to_increment;

    int interrupt_buffer;

private:
    void debug();

    void set_interrupt_master_flag();

    void handle_interrupts(byte interrupt_data);

    std::string string_write();

public:
    explicit CPU(MMU *mmu);

    int run_boot_rom();

    void push(byte to_push);

    byte pop();

    byte read(word address, bool tick_components = true);

    void write(word address, byte value, bool tick_components = true);

    byte get(Reg reg_index);

    word get(DReg reg_index);

    void set(Reg reg_index, byte value);

    void set(DReg reg_index, word value);

    void set_pc(word address, bool flush = true);

    void set_flags(vector<Flag_Status> &flag_array);

    vector<byte> fetch(Instructions &instruction_data);

    void decode_and_execute(vector<byte> fetched, Instructions &instruction_data);

    int run_instruction_cycle();

    void tick_components();
};


#endif //DMGB_CPU_HPP