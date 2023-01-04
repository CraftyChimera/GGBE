//
// Created by drake on 3/1/23.
//

#include "Timer.hpp"

Timer::Timer(MMU *mem_ptr) : mem_ptr(mem_ptr) {
    div_reg = 0;
    timer_counter = 0;
    timer_cutoff = 0;
    timer_control = 0;
    system_clock = 0;
    overflow = false;
    just_dispatched = false;
    old_bit = false;
}

void Timer::tick(int cycles) {

    while (cycles--) {
        if (just_dispatched)
            just_dispatched = false;

        if (overflow) {
            if (timer_counter == 0x00) {
                just_dispatched = true;
                timer_counter = timer_cutoff;

                byte interrupt_flags = timer_read(interrupt_request_address);
                interrupt_flags = interrupt_flags | (1 << 2);
                timer_write(interrupt_request_address, interrupt_flags);
            }
            overflow = false;
        }

        bit new_bit = select_input_bit();
        if (old_bit && !new_bit)
            increment_timer_counter();
        old_bit = new_bit;

        system_clock += 4;
    }

    set_registers();

}

bool Timer::select_input_bit() const {
    bool tac_enable = timer_control & (1 << 2);
    if (!tac_enable)
        return false;

    byte selector = timer_control & 0x3;
    switch (selector) {
        case 0x00:
            return system_clock & (1 << 9);
        case 0x01:
            return system_clock & (1 << 3);
        case 0x02:
            return system_clock & (1 << 5);
        case 0x03:
            return system_clock & (1 << 7);
        default:
            return false;
    }
}

void Timer::increment_timer_counter() {
    if (timer_counter == 0xFF)
        overflow = true;
    timer_counter++;
}

void Timer::set_registers() {
    if (mem_ptr->tima_write) {
        if (!just_dispatched)
            timer_counter = timer_read(timer_counter_address);
    }

    if (mem_ptr->reset_timer) {
        mem_ptr->reset_timer = false;
        system_clock = 0;
    }

    timer_control = timer_read(timer_control_address);
    timer_cutoff = timer_read(timer_modulo_address);

    div_reg = system_clock >> 8;
    timer_write(div_address, div_reg);
    timer_write(timer_counter_address, timer_counter);
}

byte Timer::timer_read(word address) {
    return mem_ptr->read(address);
}

void Timer::timer_write(word address, byte value) {
    mem_ptr->write(address, value);
}