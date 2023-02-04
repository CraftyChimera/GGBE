//
// Created by drake on 27/8/22.
//

#include "Console.hpp"

Console::Console(vector<byte> &data) : mmu(data), cpu(&mmu), renderer(&mmu) {
    open = true;
}


void Console::handle_event() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT: {
                open = false;
                return;
            }
        }
    }
}

void Console::loop() {
    while (open) {
        int cycles = 0;
        for (int cycles_accumulated = 0; cycles_accumulated < 70224; cycles_accumulated += cycles) {
            cycles = cpu.run_instruction_cycle();
            renderer.update(cycles);
        }
        handle_event();
    }
}

void Console::run_boot_rom() {
    while (true) {
        auto cycles = cpu.run_boot_rom();
        if (cycles == -1)
            return;
    }
}

void Console::run() {
    run_boot_rom();
    loop();
}