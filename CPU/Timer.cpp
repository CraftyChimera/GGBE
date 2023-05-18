//
// Created by drake on 3/1/23.
//

#include "Timer.hpp"

Timer::Timer(MMU *mem_ptr) : mem_ptr(mem_ptr) {
    tima_reg = 0;
    tma_reg = 0;
    tac_reg = 0;
    system_clock = 0;
    cycles_to_irq = 0;
    old_bit = false;
}

void Timer::tick(int cycles) {
    check_and_get_registers();

    while (cycles > 0) {
        cycles--;
        system_clock += 4;
        if (cycles_to_irq > 0)
            cycles_to_irq--;

        detect_edge_and_increment_timer();
        check_and_handle_irq();
    }
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
    auto tima_reg_old = tima_reg;
    if (old_bit && !new_bit) {
        tima_reg++;
    }

    old_bit = new_bit;
    if (tima_reg == 0x00 && tima_reg_old == 0xFF) {
        cycles_to_irq = 2;
    }
}

void Timer::set_registers() {
    mem_ptr->tac_write = false;
    mem_ptr->div_write = false;
    mem_ptr->tma_write = false;
    mem_ptr->tima_write = false;

    timer_write(div_address, system_clock >> 8);
    timer_write(tima_address, tima_reg);
}

void Timer::check_and_get_registers() {
    if (mem_ptr->div_write) {
        system_clock = 0;
        detect_edge_and_increment_timer();
    }

    if (mem_ptr->tima_write) {
        mem_ptr->tima_write = false;
        if (cycles_to_irq != 1) {
            tima_reg = timer_read(tima_address);
            cycles_to_irq = 0;
        }
    }

    if (mem_ptr->tma_write) {
        mem_ptr->tma_write = false;
        tma_reg = timer_read(tma_address);
        if (cycles_to_irq == 1)
            tima_reg = tma_reg;
    }
    if (mem_ptr->tac_write) {
        tac_reg = timer_read(tac_address);
        detect_edge_and_increment_timer();
    }
}

byte Timer::timer_read(word address) {
    return mem_ptr->read(address);
}

void Timer::timer_write(word address, byte value) {
    mem_ptr->io_regs.at(address - 0xFF00) = value;
}

void Timer::check_and_handle_irq() {
    if (cycles_to_irq == 1) {
        tima_reg = tma_reg;
        raise_interrupt();
    }

    if (cycles_to_irq == 2) {
        if (mem_ptr->tac_write || mem_ptr->div_write) {
            tima_reg = tma_reg;
            raise_interrupt();
            cycles_to_irq = 0;
        }
    }
}