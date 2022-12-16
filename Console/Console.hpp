//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#include "../Base/Constants.hpp"
#include "../CPU/Cpu.hpp"
#include "../GPU/Gpu.hpp"
#include "../MMU/Mmu.hpp"
#include "../Cartridge/Cartridge.hpp"
#include "../Boot/Boot.hpp"

class GPU;

class Cartridge;

class CPU;

class Console {
    MMU mmu;
    CPU cpu;
    GPU renderer;

public:
    Console();

    void run(vector<byte> &data);

private:
    void loop();
};

#endif //DMGB_CONSOLE_HPP