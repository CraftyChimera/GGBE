//
// Created by drake on 27/8/22.
//

#ifndef GGBE_CONSOLE_HPP
#define GGBE_CONSOLE_HPP

#include "Constants.hpp"
#include "SDL.h"
#include "../CPU/Cpu.hpp"
#include "../GPU/Gpu.hpp"
#include "../Bus/Bus.hpp"

class Console {

public:
    Bus bus;

    int cycles_left_till_end_of_frame;

private:
    CPU cpu;

    GPU gpu;

    bool open;

public:

    explicit Console(vector<byte> &data);

    void run();

    void tick();

private:
    void init_sdl();

    std::unordered_map<int, int> keys_pressed_map;

    void handle_event();
};

#endif //GGBE_CONSOLE_HPP