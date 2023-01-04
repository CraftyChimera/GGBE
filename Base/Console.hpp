//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "Constants.hpp"

#include "../CPU/Cpu.hpp"
#include "../GPU/Gpu.hpp"
#include "../MMU/Mmu.hpp"
#include "../Timer/Timer.hpp"

class Console {
    MMU mmu;
    CPU cpu;
    GPU renderer;
    Timer timer;

public:
    explicit Console(vector<byte> &data);

    void run();

    void run_boot_rom();

private:
    void loop();
};

#endif //DMGB_CONSOLE_HPP