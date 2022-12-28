//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "Constants.hpp"
#include "Boot.hpp"

#include "../CPU/Cpu.hpp"
#include "../GPU/Gpu.hpp"
#include "../MMU/Mmu.hpp"

class Console {
    MMU mmu;
    CPU cpu;
    GPU renderer;

public:
    explicit Console(vector<byte> &data);

    void run();

private:
    void loop();
};

#endif //DMGB_CONSOLE_HPP