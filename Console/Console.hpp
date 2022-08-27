//
// Created by drake on 27/8/22.
//

#ifndef DMGB_CONSOLE_HPP
#define DMGB_CONSOLE_HPP

#endif //DMGB_CONSOLE_HPP

#include "../Base/Common.hpp"
#include "../CPU/Cpu.hpp"
#include "../Cartridge/Cartridge.hpp"

constexpr auto memory_map_size = 0x10000;

class Console {
    array<byte, memory_map_size> memory;
    CPU cpu;
    Cartridge cartridge;
    byte ROM_BANK_NUMBER, RAM_BANK_NUMBER;

public:
    Console() : memory(array<byte, memory_map_size>{0}), ROM_BANK_NUMBER(0), RAM_BANK_NUMBER(0) {};

//TODO Complete the Memory_Map method which maps address of memory_map to the right ROM/RAM bank
    void boot(vector<byte> &data) {
        cartridge.init(data);
    }
};