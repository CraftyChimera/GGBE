//
// Created by drake on 1/10/22.
//

#include "Objects.hpp"

void Object::load(std::array<byte, memory_map_size> &data, int start) {
    for (int row = 0; row < 8; row++) {
        byte first = data[start + 2 * row], second = data[start + 2 * row + 1];
        int bitmask = 7;
        for (int bit_i = 0; bit_i < 8; bit_i++, bitmask--) {
            byte first_bit_i = first & (1 << bitmask), second_bit_i = second & (1 << bitmask);
            pixel_data[row][bit_i] = (second_bit_i << 1) + first_bit_i;
        }
    }
}