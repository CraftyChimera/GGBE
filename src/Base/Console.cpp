//
// Created by drake on 27/8/22.
//

#include "Console.hpp"

Console::Console(vector<byte> &data) : bus(data), cycles_left_till_end_of_frame(0), gpu(&bus), cpu(this), open(true) {
    init_sdl();
    keys_pressed_map[SDLK_RIGHT] = 0;
    keys_pressed_map[SDLK_LEFT] = 1;
    keys_pressed_map[SDLK_UP] = 2;
    keys_pressed_map[SDLK_DOWN] = 3;

    keys_pressed_map[SDLK_z] = 4;
    keys_pressed_map[SDLK_x] = 5;
    keys_pressed_map[SDLK_a] = 6;
    keys_pressed_map[SDLK_s] = 7;
}

void Console::handle_event() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        auto key_code = e.key.keysym.sym;
        switch (e.type) {
            case SDL_QUIT: {
                open = false;
                return;
            }
            case SDL_KEYDOWN: {
                if (key_code == SDLK_b) {
                    // cpu.start_logging = true;
                    // std::cout << "Started Logging\n";
                }
                if (key_code == SDLK_q)
                    exit(0);

                if (keys_pressed_map.count(key_code))
                    cpu.keys_pressed[keys_pressed_map[key_code]] = true;

                break;
            }
            case SDL_KEYUP: {
                if (keys_pressed_map.count(key_code))
                    cpu.keys_pressed[keys_pressed_map[key_code]] = false;
                break;
            }
        }
    }
}

void Console::run() {
    while (open) {
        cycles_left_till_end_of_frame = 456 * 154;
        while (cycles_left_till_end_of_frame > 0)
            cpu.run_instruction_cycle();
        handle_event();
    }
}

void Console::tick() {
    gpu.tick(1);
    cycles_left_till_end_of_frame--;
}

void Console::init_sdl() {
    int flags = SDL_INIT_VIDEO;
    if (SDL_Init(flags) < 0) return;

    auto &window_ref = gpu.window;
    auto &renderer_ref = gpu.renderer;
    auto &texture_ref = gpu.texture;

    window_ref = SDL_CreateWindow("GGBE", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, screen_width,
                                  screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window_ref == nullptr)
        return;

    renderer_ref = SDL_CreateRenderer(window_ref, -1, 0);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // make the scaled rendering look smoother.
    SDL_RenderSetLogicalSize(renderer_ref, screen_width, screen_height);

    texture_ref = SDL_CreateTexture(renderer_ref,
                                    SDL_PIXELFORMAT_RGB24,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    screen_width, screen_height);
}

Console::~Console() {
    SDL_Quit();
}
