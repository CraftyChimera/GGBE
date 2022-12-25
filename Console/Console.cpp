//
// Created by drake on 27/8/22.
//

#include "Console.hpp"
#include <set>

Console::Console() : mmu(), cpu(&mmu), renderer(&mmu) {}

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

        static std::set<byte> read;

        /*if (mmu.read(0xff02) == 0x81) {
            auto value_to_be_read = mmu.read(0xff01);
            if (!read.count(value_to_be_read)) {
                std::cout << std::hex << (int) mmu.read(0xff01) << " - " << std::dec << (int) mmu.read(0xff01) << "\n";
                mmu.write(0xff02, 0);
                read.insert(value_to_be_read);
            }
        }*/

    }
}

void Console::run(vector<byte> &data) {
    mmu.init_data(data);
    Boot::boot(&mmu);
    loop();
}