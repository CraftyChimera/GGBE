//
// Created by drake on 15/12/22.
//

#include "Boot.hpp"

void init_audio(MMU *mem_ptr) {

}

void play_sound() {

}

void double_bits_and_write(word &hl, byte &b, MMU *mem_ptr) {
    // b = b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_1 -> b_3 b_2 b_1 b_0 0 0 0 0, c -> b_7 b_7 b_6 b_6 b_5 b_5 b_4 b_4
    byte c = 0;
    for (int i = 3; i >= 0; i--) {
        bool msb = b & (1 << 7);
        c += (3 * msb) << (2 * i);
        b <<= 1;
    }
    for (int iteration_no = 0; iteration_no < 2; iteration_no++) {
        mem_ptr->write(hl, c);
        hl += 2;
    }
}

word load_logo(MMU *mem_ptr) {
    word de = 0x104, hl = 0x8010;
    while (de != 0x134) {
        byte a = mem_ptr->read(de);
        double_bits_and_write(hl, a, mem_ptr);
        double_bits_and_write(hl, a, mem_ptr);
        de++;
    }
    return hl;
}

void load_trademark(word hl, MMU *mem_ptr) {
    std::array<byte, 8> trademark = {0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C};
    for (int idx = 0; idx < 8; idx++) {
        byte a = trademark[idx];
        mem_ptr->write(hl, a);
        hl += 2;
    }
}

void init_tile_map(MMU *mem_ptr) {
    byte a = 0x19, c = 0x0c;
    mem_ptr->write(0x9910, a);
    word hl = 0x992f;

    while (true) {
        Loop:
        a--;
        if (a == 0) return;
        mem_ptr->write(hl--, a);
        c--;
        if (c != 0) goto Loop;
        ((hl >>= 4) <<= 4) += 0x0f;
    }
}

void animate(MMU *mem_ptr) {
    byte d = (-119) & 0xFF, c = 15;
    constexpr word scy = 0xFF42;
    constexpr word bgp = 0xFF47;

    while (c != 0) {
        byte a = d;
        byte msb = a & (1 << 7);
        (a >>= 1) += msb;
        (a >>= 1) += msb;
        mem_ptr->write(scy, a);
        a = d;
        d = a + c;

        if (c == 8) mem_ptr->write(bgp, 0xA8);
        c--;
    }
    mem_ptr->write(bgp, 0xFC);
}

void Boot::boot(MMU *mem_ptr) {
    constexpr word bgp = 0xFF47;
    constexpr word scy = 0xFF42;
    constexpr word lcd = 0xFF40;
    
    init_audio(mem_ptr);
    mem_ptr->write(bgp, 0x54);//BGP palette
    word hl = load_logo(mem_ptr);
    load_trademark(hl, mem_ptr);
    init_tile_map(mem_ptr);

    mem_ptr->write(scy, 30);
    mem_ptr->write(lcd, 0x91);

    animate(mem_ptr);
    play_sound();
}