//
// Created by drake on 27/8/22.
//

#include "Console.hpp"

Console::Console(vector<byte> &data) : cpu(this), renderer(&mmu), open(true), mmu(data),
                                       cycles_left_till_end_of_frame(0) {
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
                if (key_code == SDLK_b) {
                    cpu.start_logging = true;
                    std::cout << "Started Logging\n";
                }
                if (key_code == SDLK_q)
                    exit(0);
                if (keys_pressed_map.count(key_code))
                    cpu.keys_pressed[keys_pressed_map[key_code]] = true;
                break;
            }
            case SDL_KEYUP: {
                if (keys_pressed_map.count(key_code))
                    cpu.keys_pressed[keys_pressed_map[key_code]] = false;
                break;
            }
        }
    }
}

void Console::run() {
    while (open) {
        cycles_left_till_end_of_frame = 456 * 154;
        while (cycles_left_till_end_of_frame > 0) {
            if (cpu.is_boot)
                cpu.run_boot_rom();
            else
                cpu.run_instruction_cycle();
        }
        handle_event();
    }
}

void Console::tick_components() {
    renderer.update(1);
}