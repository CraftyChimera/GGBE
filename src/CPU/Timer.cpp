//
// Created by drake on 3/1/23.
//

#include "Timer.hpp"

Timer::Timer(Bus *mem_ptr) : ptr_to_bus(mem_ptr) {
    tima_reg = 0;
    tma_reg = 0;
    tac_reg = 0;
    system_clock = 0;
    cycles_to_irq = 0;
    old_bit = false;
}

void Timer::tick() {
    check_and_get_registers();

    system_clock += 4;
    if (cycles_to_irq > 0)
        cycles_to_irq--;

    detect_edge_and_increment_timer();
    check_and_handle_irq();
    set_registers();
}

void Timer::raise_interrupt() {
    byte interrupt_flags = timer_read(if_address);
    interrupt_flags = interrupt_flags | (1 << 2);
    timer_write(if_address, interrupt_flags);
}

bool Timer::select_input_bit() const {
    bool tac_enable = tac_reg & 4;
    if (!tac_enable)
        return false;

    byte selector = tac_reg & 3;
    switch (selector) {
        case 0x00:
            return system_clock & (1 << 9);
        case 0x03:
            return system_clock & (1 << 7);
        case 0x02:
            return system_clock & (1 << 5);
        case 0x01:
            return system_clock & (1 << 3);
        default:
            return false;
    }
}

void Timer::detect_edge_and_increment_timer() {
    bit new_bit = select_input_bit();

    if (old_bit && !new_bit) {
        tima_reg++;
        if (tima_reg == 0x00)
            cycles_to_irq = 2;
    }

    old_bit = new_bit;
}

void Timer::set_registers() {
    ptr_to_bus->tac_write = false;
    ptr_to_bus->div_write = false;
    ptr_to_bus->tma_write = false;
    ptr_to_bus->tima_write = false;

    timer_write(div_address, system_clock >> 8);
    timer_write(tima_address, tima_reg);
}

void Timer::check_and_get_registers() {
    if (ptr_to_bus->div_write) {
        system_clock = 0;
        detect_edge_and_increment_timer();
    }

    if (ptr_to_bus->tima_write) {
        if (cycles_to_irq != 1) {
            tima_reg = timer_read(tima_address);
            cycles_to_irq = 0;
        }
    }

    if (ptr_to_bus->tma_write) {
        tma_reg = timer_read(tma_address);
        if (cycles_to_irq == 1)
            tima_reg = tma_reg;
    }
    if (ptr_to_bus->tac_write) {
        tac_reg = timer_read(tac_address);
        detect_edge_and_increment_timer();
    }
}

byte Timer::timer_read(word address) {
    return ptr_to_bus->read(address);
}

void Timer::timer_write(word address, byte value) {
    ptr_to_bus->write(address, value, false);
}

void Timer::check_and_handle_irq() {
    if (cycles_to_irq == 0)
        return;

    if (cycles_to_irq == 1) {
        tima_reg = tma_reg;
        raise_interrupt();
    }

    if (cycles_to_irq == 2) {
        if (ptr_to_bus->tac_write || ptr_to_bus->div_write) {
            tima_reg = tma_reg;
            raise_interrupt();
            cycles_to_irq = 0;
        }
    }
}