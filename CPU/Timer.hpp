//
// Created by drake on 3/1/23.
//

#ifndef DMGB_TIMER_HPP
#define DMGB_TIMER_HPP

#include "../MMU/Mmu.hpp"
#include "../Base/Constants.hpp"

using bit = bool;

class Timer {
    byte tma_reg;
    byte tac_reg;
    bit old_bit;
    MMU *mem_ptr;

public:
    int cycles_to_irq;

    uint16_t system_clock;

    byte tima_reg;

    explicit Timer(MMU *mem_ptr);

    void tick(int cycles);

private:
    [[nodiscard]] bool select_input_bit() const;

    void detect_edge_and_increment_timer();

    byte timer_read(word address);

    void timer_write(word address, byte value);

    void set_registers();

    void raise_interrupt();

    void check_and_get_registers();

    void check_and_handle_irq();
};

#endif //DMGB_TIMER_HPP
