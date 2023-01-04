//
// Created by drake on 3/1/23.
//

#ifndef DMGB_TIMER_HPP
#define DMGB_TIMER_HPP

#include "../MMU/Mmu.hpp"
#include "../Base/Constants.hpp"

using bit = bool;

class Timer {
    byte div_reg;
    byte timer_counter;
    byte timer_cutoff;
    byte timer_control;
    word system_clock;
    bool overflow;
    bool just_dispatched;
    bit old_bit;
    MMU *mem_ptr;

public:
    explicit Timer(MMU *mem_ptr);

    void tick(int cycles);

    [[nodiscard]] bool select_input_bit() const;

    void increment_timer_counter();

    byte timer_read(word address);

    void timer_write(word address, byte value);

    void set_registers();

    void set_interrupt();
};

#endif //DMGB_TIMER_HPP
