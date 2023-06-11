//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "Constants.hpp"

#include "../CPU/Cpu.hpp"
#include "../GPU/Gpu.hpp"
#include "../MMU/Mmu.hpp"

class Console {
    CPU cpu;

    GPU renderer;

    bool open;
    
public:
    MMU mmu;

    int cycles_left_till_end_of_frame;

    explicit Console(vector<byte> &data);

    void run();

    void tick_components();

private:
    std::unordered_map<int, int> keys_pressed_map;

    void handle_event();
};

#endif //DMGB_CONSOLE_HPP