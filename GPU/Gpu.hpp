//
// Created by drake on 1/10/22.
//

#ifndef DMGB_GPU_HPP
#define DMGB_GPU_HPP

#include "Utility.hpp"
#include "Objects.hpp"
#include <iostream>

class GPU {
    std::array<std::array<Object, 128>, 3> tile_data;

public:
    bool stale;

    GPU() noexcept;

    void render(std::array<byte, memory_map_size> &memory);

    void load_data(std::array<byte, memory_map_size> &memory);
};

#endif //DMGB_GPU_HPP
