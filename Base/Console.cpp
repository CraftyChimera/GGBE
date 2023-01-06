//
// Created by drake on 27/8/22.
//

#include "Console.hpp"

Console::Console(vector<byte> &data) : mmu(data), cpu(&mmu), renderer(&mmu) {}

void Console::loop() {
    SDL_Event e;
    bool open = true;

    while (open) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    open = false;
                    break;
                }
                case SDL_WINDOWEVENT: {
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        renderer.resize();
                    }
                }
            }
        }

        auto cycles = cpu.run_instruction_cycle();
        renderer.update(cycles);
    }
}

void Console::run_boot_rom() {
    cpu.run_boot_rom();
}

void Console::run() {
    run_boot_rom();
    loop();
}