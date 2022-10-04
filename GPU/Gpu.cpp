//
// Created by drake on 1/10/22.
//

#include "Gpu.hpp"

GPU::GPU() noexcept: tile_data(std::array<std::array<Object, 128>, 3>{}), stale(false) {}

void GPU::load_data(std::array<byte, memory_map_size> &memory) {
    for (auto block_number = 0; block_number < 3; block_number++) {
        for (auto block_id = 0; block_id < 128; block_id++) {
            tile_data[block_number][block_id].load(memory, vram_start + 16 * (128 * block_number + block_id));
        }
    }
}


void GPU::render(std::array<byte, memory_map_size> &memory) {
    if (stale) {
        load_data(memory);
        stale = false;
    }

    
}