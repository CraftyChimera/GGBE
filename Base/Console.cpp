//
// Created by drake on 27/8/22.
//

#include "Console.hpp"

Console::Console(vector<byte> &data) : mmu(data), cpu(&mmu), renderer(&mmu), open(true) {
    keys_pressed_map[SDLK_RIGHT] = 0;
    keys_pressed_map[SDLK_LEFT] = 1;
    keys_pressed_map[SDLK_UP] = 2;
    keys_pressed_map[SDLK_DOWN] = 3;

    keys_pressed_map[SDLK_z] = 4;
    keys_pressed_map[SDLK_x] = 5;
    keys_pressed_map[SDLK_c] = 6;
    keys_pressed_map[SDLK_v] = 7;
}

void Console::handle_event() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        auto key_code = e.key.keysym.sym;
        switch (e.type) {
            case SDL_QUIT: {
                open = false;
                return;
            }
            case SDL_KEYDOWN: {
                if (keys_pressed_map.count(key_code))
                    cpu.keys_pressed[keys_pressed_map[key_code]] = false;
                break;
            }
            case SDL_KEYUP: {
                if (keys_pressed_map.count(key_code))
                    cpu.keys_pressed[keys_pressed_map[key_code]] = true;
                break;
            }
        }
    }
}

void Console::loop() {
    while (open) {
        int cycles = 0;
        for (int cycles_accumulated = 0; cycles_accumulated < 456 * 154; cycles_accumulated += cycles) {
            cycles = cpu.run_instruction_cycle();
            renderer.update(cycles);
        }
        handle_event();
    }
}

void Console::run_boot_rom() {
    while (true) {
        auto cycles = cpu.run_boot_rom();
        if (cycles == -1) {
            return;
        }
    }
}

void Console::run() {
    run_boot_rom();
    loop();
}